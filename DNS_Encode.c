//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

#include "DNS_Encode.h"
/*
int main(int argc, char* argv[])
{
    char msg[128];

	printf("Enter the msg to encode: ");
	fgets(msg, 128, stdin);

	// Remove trailing newline, if there is.
	if ((strlen(msg) > 0) && (msg[strlen(msg)-1] == '\n'))
	{msg[strlen(msg)-1] = '\0';}
	
	char *dns = malloc(128);
	printf("Original = '%s'\n", msg);
	ChangetoDnsNameFormat(dns, msg); 

	printf("Encoded = '%s'\n", dns);
	free(dns);
	return 0;
}
*/

/* Split the message into sections of 63 bits (because in QNAME each word between the dots only has 64 bits)
 * The first word can only have 62 bits ("d" in front and "." behind)
 * The others can have 63 ("." behind)
 */

char* DNS_Split(char* msg)
{
	char *splited = NULL;

	/* Encode using Base32 */

	printf("Original = '%s'\n", msg); 

	int required = strlen(msg) + strlen(msg)/63 + 1;
	splited = (char*) malloc(required);
	int comp_spl = 0;
	int comp_msg = 0;
	int size_pack = 62;

	while (1)
	{
		for (int j = 0; j < size_pack; j++)
		{
			if (comp_msg < strlen(msg))
			{
				splited[comp_spl] = msg[comp_msg];
				comp_spl++;
				comp_msg++;
			}
			else {return splited;}
		}
		if (comp_msg + 1 < strlen(msg))
		{
			splited[comp_spl] = '.';
			comp_spl++;
		}
		else {return splited;}
		size_pack = 63;
	}

    printf("Splited = '%s'\n", splited);
    return splited;
}


/*
The following is modified from :
https://gist.github.com/fffaraz/9d9170b57791c28ccda9255b48315168
which orginally performs a DNS Query from input hostname

Author : Silver Moon (m00n.silv3r@gmail.com)
Dated : 29/4/2009
*/

/*
 * This will convert www.google.com to 3www6google3com 
 * */
void ChangetoDnsNameFormat(char* dns, char* host) 
{
    int run_dns = 0, run_host = 0;
    strcat(host,".");
     
    for(int i = 0; i < strlen(host); i++) 
    {
        if(host[i]=='.') 
        {
            char buf[2];
            memset(buf, 0, 2);
            sprintf(buf, "%d", i-run_host);
            strcat(dns, buf);
            run_dns += strlen(buf);
            for(int j=0; j<i-run_host; j++) 
            {
                dns[run_dns] = host[run_host+j];
                run_dns++;
            }
            run_host = i+1;
        }
    }
    dns[run_dns] = '\0';
}