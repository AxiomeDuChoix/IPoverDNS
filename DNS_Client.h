/*Create 2 threads
 *1. Sending thread: listen from tap0, transform the msg into an DNS request and send it to the DNS server
 *2. Receiving thread: perform an DNS request permanently in order to get the answers from the DNS server
 */

#ifndef DNS_CLIENT_H
#define DNS_CLIENT_H

//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc
#include <sys/socket.h> //you know what this is for
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

#include "DNS_Query.h"
#include "DNS_Listen.h"

struct sockfd_ipDNS_host {
	int tapfd;
	int sockfd;
	char *ip_dns_server;
	char *host;
};

void *sending(void *sih_void);
void *receiving(void *sih_void);
int tun_alloc(char *dev, int flags);
int cread(int fd, char *buf, int n);
int cwrite(int fd, char *buf, int n);
int read_n(int fd, char *buf, int n);

#endif