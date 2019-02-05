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

#include "DNS_Client.h"
#include "DNS_Query.h"

#define T_A 1 //Ipv4 address
#define T_NS 2 //Nameserver
#define T_CNAME 5 // canonical name
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 //Mail server

#define MAX_SZ 32768

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Client must have exactly 2 parameters as input: Hostname and DNS server's IP.\n");
		return (EXIT_FAILURE);
	}

	char *host = argv[1];
	char *ip_dns_server = argv[2];

	//create a socket
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0); //IPv4 (AF_INET6 if IPv6)
	if (sockfd >= 0) {
		printf("Socket created.\n");
	}
	else {
		printf("Error creating socket.\n");
	}

	//assign name to socket (bind function)
	struct sockaddr_in address;
	memset(&address.sin_zero, 0, sizeof(address.sin_zero));
	address.sin_family = AF_INET; //IPv4
	address.sin_port = htons(80);
	
	const struct sockaddr *addr = (struct sockaddr*) &address;
	
	if (bind(sockfd, addr, sizeof(*addr))) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	//arguments of sending & receiving
	struct sockfd_ipDNS_host sih_init;

	sih_init.sockfd = sockfd;
	sih_init.ip_dns_server = ip_dns_server;
	sih_init.host = host;

	struct sockfd_ipDNS_host *sih;
	sih = &sih_init;
	
	//create threads in order to send messages while receiving data
	pthread_t sending_thread;
	pthread_t receiving_thread;

	while (1) {
		if (pthread_create(&sending_thread, NULL, &sending, (void*) sih)) {
			fprintf(stderr, "Error creating sending thread.\n");
			return (EXIT_FAILURE);
		}
		if (pthread_join(sending_thread, NULL)) {
			fprintf(stderr, "Error joining sending thread.\n");
			return (EXIT_FAILURE);
		}
		if (pthread_create(&receiving_thread, NULL, &receiving, (void*) sih)) {
			fprintf(stderr, "Error creating receiving thread.\n");
			return (EXIT_FAILURE);
		}
	}
	
	pthread_exit(NULL);

	//close socket
	if (close(sockfd) == 0) {
		printf("Socket closed.\n");
		return (EXIT_SUCCESS);
	}
	else {
		printf("Error closing socket.\n");
		return (EXIT_FAILURE);
	}
}

void *sending(void *sih_void)
{
	struct sockfd_ipDNS_host *sih = (struct sockfd_ipDNS_host*) sih_void;
	int sockfd = sih->sockfd;
	char* ip_dns_server = sih->ip_dns_server;
	char* host = sih->host;
	
	/* listen from tap0 */
	char msg[MAX_SZ];
	memset(msg, 0, MAX_SZ);

	int nb_bytes_recved;
	struct sockaddr from;
	socklen_t fromlen = sizeof(from);
	
	printf("Listening from tap0...\n");
	nb_bytes_recved = recvfrom(sockfd, msg, MAX_SZ, 0, &from, &fromlen);

	char* pointer = msg;
	pointer += nb_bytes_recved - 1;
	*pointer = '\0';

	printf("Message to encode: '%s' (%d bytes)\n", msg, nb_bytes_recved-1);

	/* transform the msg into a DNS request and send it to the DNS server */
	void* sockfd_void;
	sockfd_void = &sockfd;
	DNS_Query(sockfd_void, msg, host, ip_dns_server, T_A);
}

void *receiving(void *sih_void)
{
	struct sockfd_ipDNS_host *sih = (struct sockfd_ipDNS_host*) sih_void;
	int sockfd = sih->sockfd;
	char* ip_dns_server = sih->ip_dns_server;
	char* host = sih->host;

	/* ask if the DNS server has a packet to send back*/
	void* sockfd_void;
	sockfd_void = &sockfd;

	
	DNS_Query(sockfd_void, "", host, ip_dns_server, T_A);
	sleep(3);
}