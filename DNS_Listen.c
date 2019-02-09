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

#include "DNS_Query.h"
#include "DNS_Listen.h"
#include "DNS_Decode.h"
#include "New_from_Binary.h"

#define T_A 1 //Ipv4 address
#define T_NS 2 //Nameserver
#define T_CNAME 5 // canonical name
#define T_SOA 6 /* start of authority zone */
#define T_PTR 12 /* domain name pointer */
#define T_MX 15 //Mail server

#define MAX_SZ 32768

char* Listen(void* sockfd_void, int len_qname, char *ip_dns_server)
	{	
		char buf[MAX_SZ*2];
		struct RES_RECORD answer;

	    int sockfd = *(int *) sockfd_void;
	    struct sockaddr_in a;

	    struct DNS_PACKET *dnspacket = NULL;

	    /* DNS server parameters */
	    struct sockaddr_in dest;
	    dest.sin_family = AF_INET;
	    dest.sin_port = htons(53); //DNS uses the port 53
	    dest.sin_addr.s_addr = inet_addr(ip_dns_server);
	    int i = sizeof dest;
	    
	    /* Receiving answer */
	    printf("\nReceiving answer...");

	    if (recvfrom(sockfd, (char*)buf, 65536, 0, (struct sockaddr*)&dest, (socklen_t*)&i) < 0)
	    {
	        perror("recvfrom failed");
	    }
	    printf("Done");

	    dnspacket = new_from_binary(buf, len_qname);

	    printf("\nThe response contains : ");
	    printf("\n %d Questions.", ntohs((dnspacket->header)->q_count));
	    printf("\n %d Answers.", ntohs((dnspacket->header)->ans_count));
	    printf("\n %d Authoritative Servers.", ntohs((dnspacket->header)->auth_count));
	    printf("\n %d Additional records.\n\n", ntohs((dnspacket->header)->add_count));
	 
	    //Start reading answers
	    struct RES_RECORD reader[sizeof(struct RES_RECORD)]; 
	    memcpy(reader, dnspacket->record, sizeof(struct RES_RECORD));
		
        answer.name = ReadName(reader->name, buf);
        answer.resource = reader->resource;
 
        if (ntohs(answer.resource->type) == 1) //if its an ipv4 address
        {
            answer.rdata = (char*) malloc(ntohs(answer.resource->data_len));
 
            for(int j=0; j<ntohs(answer.resource->data_len); j++)
            {
                answer.rdata[j]=(reader->rdata)[j];
            }
 
            answer.rdata[ntohs(answer.resource->data_len)] = '\0';
        }
        else
        {
            answer.rdata = ReadName(reader->name, buf);
        }
	 	
	 	//print answer
        printf("Name: %s ", answer.name);
         
        char* received = NULL;

        if (ntohs(answer.resource->type) == 5) 
        {
            if (ntohs(answer.resource->type) == 5) //Canonical name for an alias
        	{
	            char* msg = DNS_Unsplit(answer.name);
	            received = msg + 1;
	            printf("Msg received: '%s'\n", received);
        	}
        }
        else {printf("Msg received type error.\n");}
 
        printf("\n");

		free(answer.name);
		free(answer.rdata);
		free(dnspacket->header);
		free(dnspacket->qname);
		free(dnspacket->question);
		free(dnspacket->record);
		free(dnspacket);

		return received;
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
