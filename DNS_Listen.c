/*Listen from the DNS_Server after sending messages*/

/*
Modified from :
https://gist.github.com/fffaraz/9d9170b57791c28ccda9255b48315168
which orginally performs a DNS Query from input hostname

Author : Silver Moon (m00n.silv3r@gmail.com)
Dated : 29/4/2009
*/

//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc
#include <sys/socket.h> //you know what this is for
#include <arpa/inet.h> //inet_addr , inet_ntoa , ntohs etc
#include <netinet/in.h>
#include <unistd.h> //getpid

#include "Query.h"
#include "DNS_Listen.h"
#include "DNS_Decode.h"

#define MAX_SZ 32768

void Listen(void* sockfd_void, int len_qname, char *ip_dns_server)
	{	
		char buf[65536];
	    int i, j, stop;

	    int sockfd = *(int *) sockfd_void;
	    struct sockaddr_in a;

	    struct DNS_HEADER *dns = NULL;
	    dns = (struct DNS_HEADER *) &buf;

	    struct RES_RECORD answers[20]; //auth[20], addit[20]; //the replies from the DNS server

	    /* DNS server parameters */
	    struct sockaddr_in dest;
	    dest.sin_family = AF_INET;
	    dest.sin_port = htons(53); //DNS uses the port 53
	    dest.sin_addr.s_addr = inet_addr(ip_dns_server);
	    i = sizeof dest;
	    
	    /* Receiving answer */
	    printf("\nReceiving answer...");

	    if (recvfrom(sockfd, (char*)buf, 65536, 0, (struct sockaddr*)&dest, (socklen_t*)&i) < 0)
	    {
	        perror("recvfrom failed");
	    }
	    printf("Done");

	    //Move ahead of the dns header and the query field
	    reader = &buf[sizeof(struct DNS_HEADER) + len_qname + sizeof(struct QUESTION)];

	    printf("\nThe response contains : ");
	    printf("\n %d Questions.", ntohs(dns->q_count));
	    printf("\n %d Answers.", ntohs(dns->ans_count));
	    printf("\n %d Authoritative Servers.", ntohs(dns->auth_count));
	    printf("\n %d Additional records.\n\n", ntohs(dns->add_count));
	 
	    //Start reading answers
	    stop = 0;
	 
	    for (i=0; i<ntohs(dns->ans_count); i++)
	    {
	        answers[i].name = ReadName(reader, buf, &stop);
	        reader = reader + stop;
	 
	        answers[i].resource = (struct R_DATA*) (reader);
	        reader = reader + sizeof(struct R_DATA);
	 
	        if (ntohs(answers[i].resource->type) == 1) //if its an ipv4 address
	        {
	            answers[i].rdata = (unsigned char*) malloc(ntohs(answers[i].resource->data_len));
	 
	            for(j=0; j<ntohs(answers[i].resource->data_len); j++)
	            {
	                answers[i].rdata[j]=reader[j];
	            }
	 
	            answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';
	 
	            reader = reader + ntohs(answers[i].resource->data_len);
	        }
	        else
	        {
	            answers[i].rdata = ReadName(reader, buf, &stop);
	            reader = reader + stop;
	        }
	    }
	 	
	 	/*
	    //read authorities
	    for(i=0; i<ntohs(dns->auth_count); i++)
	    {
	        auth[i].name = ReadName(reader, buf, &stop);
	        reader += stop;
	 
	        auth[i].resource = (struct R_DATA*) (reader);
	        reader += sizeof(struct R_DATA);
	 
	        auth[i].rdata = ReadName(reader, buf, &stop);
	        reader += stop;
	    }
	 
	    //read additional
	    for(i=0;i<ntohs(dns->add_count);i++)
	    {
	        addit[i].name = ReadName(reader,buf,&stop);
	        reader += stop;
	 
	        addit[i].resource = (struct R_DATA*) (reader);
	        reader += sizeof(struct R_DATA);
	 
	        if (ntohs(addit[i].resource->type) == 1)
	        {
	            addit[i].rdata = (unsigned char*) malloc(ntohs(addit[i].resource->data_len));
	            for(j=0; j<ntohs(addit[i].resource->data_len); j++)
	            addit[i].rdata[j] = reader[j];
	 
	            addit[i].rdata[ntohs(addit[i].resource->data_len)] = '\0';
	            reader += ntohs(addit[i].resource->data_len);
	        }
	        else
	        {
	            addit[i].rdata = ReadName(reader,buf,&stop);
	            reader += stop;
	        }
	    }
	 	*/

	    //print answers
	    printf("\nAnswer Records: %d \n" , ntohs(dns->ans_count) );
	    for (i=0; i < ntohs(dns->ans_count) ; i++)
	    {
	        printf("Name: %s ", answers[i].name);
	 
	        if (ntohs(answers[i].resource->type) == T_A) //IPv4 address
	        {
	            long *p;
	            p = (long*)answers[i].rdata;
	            a.sin_addr.s_addr = (*p); //working without ntohl
	            printf("has IPv4 address: %s", inet_ntoa(a.sin_addr));
	        }
	         
	        if (ntohs(answers[i].resource->type) == 5) 
	        {
	            //Canonical name for an alias
	            printf("has alias name: %s", answers[i].rdata);
	        }
	 
	        printf("\n");
		free(answers[i].name);
		free(answers[i].rdata);
	    }
	 	
	 	/*
	    //print authorities
	    printf("\nAuthoritive Records: %d \n", ntohs(dns->auth_count) );
	    for (i = 0; i < ntohs(dns->auth_count); i++)
	    {
	        printf("Name: %s ", auth[i].name);
	        if(ntohs(auth[i].resource->type)==2)
	        {
	            printf("has nameserver : %s", auth[i].rdata);
	        }
	        printf("\n");
		free(auth[i].name);
	    }
	 
	    //print additional resource records
	    printf("\nAdditional Records : %d \n", ntohs(dns->add_count) );
	    for(i=0; i < ntohs(dns->add_count); i++)
	    {
	        printf("Name : %s ", addit[i].name);
	        if (ntohs(addit[i].resource->type) == 1)
	        {
	            long *p;
	            p = (long*) addit[i].rdata;
	            a.sin_addr.s_addr = (*p);
	            printf("has IPv4 address : %s", inet_ntoa(a.sin_addr));
		    free(addit[i].rdata);
	        }
	        printf("\n");
	    }
	    */
	}
