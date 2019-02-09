/*Listen from the DNS_Server after sending messages*/

/*
Modified from :
https://gist.github.com/fffaraz/9d9170b57791c28ccda9255b48315168
which orginally performs a DNS Query from input hostname

Author : Silver Moon (m00n.silv3r@gmail.com)
Dated : 29/4/2009
*/

#ifndef DNS_LISTEN_H
#define DNS_LISTEN_H

//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc
#include <sys/socket.h> //you know what this is for
#include <arpa/inet.h> //inet_addr , inet_ntoa , ntohs etc
#include <netinet/in.h>
#include <unistd.h> //getpid

#include "DNS_Query.h"
#include "DNS_Decode.h"
#include "New_from_Binary.h"

#define MAX_SZ 32768

char* Listen(void* sockfd_void, int len_qname, char *ip_dns_server);

#endif