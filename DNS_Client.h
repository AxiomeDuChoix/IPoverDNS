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

#include "DNS_Query.h"

struct sockfd_ipDNS_host {
	int sockfd;
	char *ip_dns_server;
	char *host;
};

void *sending(void *sih_void);
void *receiving(void *sih_void);

#endif