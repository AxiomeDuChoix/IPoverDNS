/*Create 2 threads
 *1. Sending thread: listen from tap0, transform the msg into a DNS request and send it to the DNS server
 *2. Receiving thread: perform an DNS request permanently in order to get the answers from the DNS server
 */

//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr , inet_ntoa , ntohs etc
#include <netinet/in.h>
#include <unistd.h> //getpid
#include <pthread.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "DNS_Client.h"
#include "DNS_Query.h"

#define T_A 1 //Ipv4 address
#define T_NS 2 //Nameserver
#define T_CNAME 5 // canonical name
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 //Mail server

#define MAX_SZ 32768

pthread_mutex_t lock;
int num_req = 0;

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Client must have exactly 2 parameters as input: Hostname and DNS server's IP.\n");
		return (EXIT_FAILURE);
	}

	char *host = argv[1];
	char *ip_dns_server = argv[2];

	int tap_fd;
  	char if_name[MAX_SZ] = "";

  	/* initialize tun/tap interface */
  	strncpy(if_name, "tap0", MAX_SZ-1);
  	if ((tap_fd = tun_alloc(if_name, IFF_TAP | IFF_NO_PI)) < 0) {
    	printf("Error connecting to interface %s.\n", if_name);
    	return (EXIT_FAILURE);
  	}
	printf("Successfully connected to interface %s.\n", if_name);

	//create a socket (to communicate with DNS server)
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0); //IPv4 (AF_INET6 if IPv6)
	if (sockfd < 0) {
		printf("Error creating socket.\n");
		return (EXIT_FAILURE);
	}
	printf("Socket created.\n");

	/*
	//assign name to socket (bind function)
	struct sockaddr_in address;
	bzero((char *) &address, sizeof(address));
	memset(&address.sin_zero, 0, sizeof(address.sin_zero));
	address.sin_family = AF_INET; //IPv4
	address.sin_port = htons(443);
	address.sin_addr.s_addr = htonl(inet_network("10.0.0.1"));
	
	const struct sockaddr *addr = (struct sockaddr*) &address;
	
	if (bind(sockfd, addr, sizeof(*addr))) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	*/

	//arguments of functions "sending" & "receiving"
	struct sockfd_ipDNS_host sih_init;

	sih_init.tapfd = tap_fd;
	sih_init.sockfd = sockfd;
	sih_init.ip_dns_server = ip_dns_server;
	sih_init.host = host;

	struct sockfd_ipDNS_host *sih = NULL;
	sih = &sih_init;

	//create threads in order to send messages while receiving data
	pthread_t sending_thread;
	pthread_t receiving_thread;

	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return (EXIT_FAILURE);
    }

	while (1) {
		if (pthread_create(&sending_thread, NULL, &sending, (void*) sih)) {
			fprintf(stderr, "Error creating sending thread.\n");
			return (EXIT_FAILURE);
		}
		if (pthread_create(&receiving_thread, NULL, &receiving, (void*) sih)) {
			fprintf(stderr, "Error creating receiving thread.\n");
			return (EXIT_FAILURE);
		}
		/*
		if (pthread_join(sending_thread, NULL)) {
			fprintf(stderr, "Error joining sending thread.\n");
			return (EXIT_FAILURE);
		}
		*/
		if (pthread_join(receiving_thread, NULL)) {
			fprintf(stderr, "Error joining receiving thread.\n");
			return (EXIT_FAILURE);
		}
	}
	
	pthread_exit(NULL);

	/*
	//close socket
	if (close(sockfd) == 0) {
		printf("Socket closed.\n");
		return (EXIT_SUCCESS);
	}
	else {
		printf("Error closing socket.\n");
		return (EXIT_FAILURE);
	}
	*/
}

void *sending(void *sih_void)
{
	pthread_mutex_lock(&lock);

	struct sockfd_ipDNS_host *sih = NULL;
	sih = (struct sockfd_ipDNS_host*) sih_void;
	int tap_fd = sih->tapfd;
	int sockfd = sih->sockfd;
	char* ip_dns_server = NULL;
	ip_dns_server = sih->ip_dns_server;
	char* host = NULL;
	host = sih->host;
	
	int ret;
    fd_set rd_set;
    FD_ZERO(&rd_set);
    FD_SET(tap_fd, &rd_set);
    uint16_t nread, nwrite, plength;

	/* listen from tap0 */
	char msg[MAX_SZ];
	memset(msg, 0, MAX_SZ);

	if(FD_ISSET(tap_fd, &rd_set))
	{
      nread = cread(tap_fd, msg, MAX_SZ);
      printf("Message to encode: '%s' (%d bytes)\n", msg, nread);
    }

	/*
	int nb_bytes_recved;
	struct sockaddr from;
	socklen_t fromlen = sizeof(from);
	
	printf("Listening from tap0...\n");
	nb_bytes_recved = recvfrom(sockfd, msg, MAX_SZ, 0, &from, &fromlen);

	char* pointer = msg;
	pointer += nb_bytes_recved - 1;
	*pointer = '\0';
	*/

	/* transform the msg into a DNS request and send it to the DNS server */
	void* sockfd_void = NULL;
	sockfd_void = &sockfd;
	
	DNS_Query(1, sockfd_void, msg, host, ip_dns_server, T_A, 0);
	
	pthread_mutex_unlock(&lock);
}

void *receiving(void *sih_void)
{
	pthread_mutex_lock(&lock);

	num_req += 1

	struct sockfd_ipDNS_host *sih = NULL;
	sih = (struct sockfd_ipDNS_host*) sih_void;
	int tapfd = sih->tapfd;
	int sockfd = sih->sockfd;
	char* ip_dns_server = NULL;
	ip_dns_server = sih->ip_dns_server;
	char* host = NULL;
	host = sih->host;

	/* ask if the DNS server has a packet to send back */
	void* sockfd_void = NULL;
	sockfd_void = &sockfd;

	/* see DNS_Query.c */
	int len = DNS_Query(0, sockfd_void, atoi(num_req), host, ip_dns_server, T_A, num_req);
	
	/* see DNS_Listen.c */
	Listen(sockfd_void, len, ip_dns_server);
	pthread_mutex_unlock(&lock);
	sleep(3);
}


/* The following code was copied from:
 * http://www.cis.syr.edu/~wedu/seed/Labs_12.04/Networking/Firewall_VPN/files/simpletun.c
 */

/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device. The caller     *
 *            needs to reserve enough space in *dev.                      *
 **************************************************************************/
int tun_alloc(char *dev, int flags) {

  struct ifreq ifr;
  int fd, err;

  if( (fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
    perror("Opening /dev/net/tun");
    return fd;
  }

  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_flags = flags;

  if (*dev) {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
    perror("ioctl(TUNSETIFF)");
    close(fd);
    return err;
  }

  strcpy(dev, ifr.ifr_name);

  return fd;
}

/**************************************************************************
 * cread: read routine that checks for errors and exits if an error is    *
 *        returned.                                                       *
 **************************************************************************/
int cread(int fd, char *buf, int n){
  
  int nread;

  if((nread=read(fd, buf, n))<0){
    perror("Reading data");
    exit(1);
  }
  return nread;
}

/**************************************************************************
 * cwrite: write routine that checks for errors and exits if an error is  *
 *         returned.                                                      *
 **************************************************************************/
int cwrite(int fd, char *buf, int n){
  
  int nwrite;

  if((nwrite=write(fd, buf, n))<0){
    perror("Writing data");
    return (EXIT_FAILURE);
  }
  return nwrite;
}

/**************************************************************************
 * read_n: ensures we read exactly n bytes, and puts those into "buf".    *
 *         (unless EOF, of course)                                        *
 **************************************************************************/
int read_n(int fd, char *buf, int n){

  int nread, left = n;

  while(left > 0) {
    if ((nread = cread(fd, buf, left))==0){
      return 0 ;      
    }else {
      left -= nread;
      buf += nread;
    }
  }
  return n;  
}