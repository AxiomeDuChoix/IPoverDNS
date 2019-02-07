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

#define MAX_SZ 32768

/*
 * Perform a DNS query by sending a packet combining msg and hostname, and return the length of qname
 * */
int DNS_Query(int nature, void* sockfd_void, char *msg, char *host, char *ip_dns_server, int query_type, int num_req)
{
    char buf[65536], msg_encoded[32768]; // Why is there a problem with msg_encoded when using malloc ?????
    char *qname = NULL, *splited = NULL;

    int s = *(int *) sockfd_void;

    struct sockaddr_in dest;

    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;

    //printf("Resolving %s...\n", host);

    dest.sin_family = AF_INET;
    dest.sin_port = htons(53); //DNS uses the port 53
    dest.sin_addr.s_addr = inet_addr(ip_dns_server);

    //Set the DNS structure to standard queries
    dns = (struct DNS_HEADER *) &buf;

    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    //Point to the query portion
    qname = (char*) &buf[sizeof(struct DNS_HEADER)];

    /* convert msg into 'd/r'+${msg}+'.'+${hostname} see DNS_Encode.c */
    splited = Encode(msg);
    
    if (nature) //real query containing information 
    {
        msg_encoded[0] = 'd';
    }
    else //fake query requesting answers
    {
        msg_encoded[0] = 'r';
    }
    //printf("%s\n", msg_encoded);

    strcat(msg_encoded, splited);
    msg_encoded[strlen(splited) + 1] = '.';
    strcat(msg_encoded, host);

    printf("Msg ready to be sent: '%s'.\n", msg_encoded);

    /* see DNS_Encode.c */
    ChangetoDnsNameFormat(qname, msg_encoded); //e.g. text: www.google.com & qname: 3www6google3com

    free(splited);

    qinfo = (struct QUESTION*) &buf[sizeof(struct DNS_HEADER) + (strlen((const char*) qname) + 1)]; //fill it

    qinfo->qtype = htons(query_type); //type of the query, A, MX, CNAME, NS etc
    qinfo->qclass = htons(1); //its internet (lol)

    int len_qname = strlen((const char*)qname) + 1;
    
    if (nature)
    {
        printf("Sending Packet...\n");    
    }
    else
    {
        printf("Sending request for data number '%d'...\n", num_req);
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