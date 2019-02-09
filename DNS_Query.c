//Change our message into a DNS Query

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
#include "DNS_Encode.h"
#include "Binary_from_New.h"

#define MAX_SZ 32768

/*
 * Perform a DNS query by sending a packet combining msg and hostname, and return the length of qname
 * */
int DNS_Query(int nature, void* sockfd_void, char *msg, char *host, char *ip_dns_server, int query_type)
{
    struct DNS_PACKET *dnspacket = NULL;
    dnspacket = (struct DNS_PACKET*) malloc(sizeof(struct DNS_PACKET));

    int s = *(int *) sockfd_void;

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53); //DNS uses the port 53
    dest.sin_addr.s_addr = inet_addr(ip_dns_server);

    // DNS_HEADER
    struct DNS_HEADER* header = (struct DNS_HEADER*) malloc(sizeof(struct DNS_HEADER));
    header = dnspacket->header;

    header->id = (unsigned short) htons(getpid());
    header->qr = 0; //This is a query
    header->opcode = 0; //This is a standard query
    header->aa = 0; //Not Authoritative
    header->tc = 0; //This message is not truncated
    header->rd = 1; //Recursion Desired
    header->ra = 0; //Recursion not available! hey we dont have it (lol)
    header->z = 0;
    header->ad = 0;
    header->cd = 0;
    header->rcode = 0;
    header->q_count = htons(1); //we have only 1 question
    header->ans_count = 0;
    header->auth_count = 0;
    header->add_count = 0;

    // Query portion
    char *qname = (char *) malloc(MAX_SZ);
    qname = dnspacket->qname;

    // convert msg into 'd/r'+${msg}+'.'+${hostname} see DNS_Encode.c
    char msg_encoded[32768];
    char *splited = NULL;
    splited = DNS_Split(msg);
    
    if (nature) //real query containing information 
    {msg_encoded[0] = 'd';}
    else //fake query requesting answers
    {msg_encoded[0] = 'r';}
    //printf("%s\n", msg_encoded);

    strcat(msg_encoded, splited);
    msg_encoded[strlen(splited) + 1] = '.';
    msg_encoded[strlen(splited) + 2] = '$';
    msg_encoded[strlen(splited) + 3] = '.';
    strcat(msg_encoded, host);

    printf("Msg ready to be sent: '%s'.\n", msg_encoded);

    /* see DNS_Encode.c */
    ChangetoDnsNameFormat(qname, msg_encoded); //e.g. text: www.google.com & qname: 3www6google3com

    free(splited);

    struct QUESTION *question = (struct QUESTION*) malloc(sizeof(struct QUESTION)); //fill it

    question->qtype = htons(query_type); //type of the query, A, MX, CNAME, NS etc
    question->qclass = htons(1); //its internet (lol)

    int len_qname = strlen(qname) + 1;
    
    char buf[2*MAX_SZ];
    strcpy(buf, binary_from_new(dnspacket));
    
    free(dnspacket);
    free(header);
    free(qname);
    free(question);

    if (nature)
    {
        printf("Sending Packet...\n");    
    }
    else
    {
        printf("Sending request for data...\n");
    }
    
    if (sendto(s, (char*)buf, sizeof(struct DNS_HEADER)+ len_qname + sizeof(struct QUESTION), 0, (struct sockaddr*)&dest, sizeof(dest)) < 0)
    {
        perror("Sendto failed.");
    }
    printf("Done.\n\n");

    return len_qname;
}


/*
 * Get the DNS servers from /etc/resolv.conf file on Linux

void get_dns_servers()
{
    FILE *fp;
    char line[200] , *p;
    if((fp = fopen("/etc/resolv.conf" , "r")) == NULL)
    {
        printf("Failed opening /etc/resolv.conf file \n");
    }
     
    while(fgets(line , 200 , fp))
    {
        if(line[0] == '#')
        {
            continue;
        }
        if(strncmp(line , "nameserver" , 10) == 0)
        {
            p = strtok(line , " ");
            p = strtok(NULL , " ");
             
            //p now is the dns ip :)
            //????
        }
    }
     
    strcpy(dns_servers[0] , "208.67.222.222");
    strcpy(dns_servers[1] , "208.67.220.220");
}
*/