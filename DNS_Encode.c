//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

#include "DNS_Encode.h"
/*
#define MAX_SZ 32768

int main(int argc, char* argv[])
{
    char msg[MAX_SZ];

	printf("Enter the msg to encode: ");
	fgets(msg, MAX_SZ, stdin);

	// Remove trailing newline, if there is.
	if ((strlen(msg) > 0) && (msg[strlen(msg)-1] == '\n'))
	{msg[strlen(msg)-1] = '\0';}

	char* splited = DNS_Split(msg);
	printf("Splited = '%s'\n", splited);
	free(splited);
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
    int lock = 0 , i;
    strcat((char*)host,".");
     
    for(i = 0 ; i < strlen((char*)host) ; i++) 
    {
        if(host[i]=='.') 
        {
            *dns++ = i-lock;
            for(;lock<i;lock++) 
            {
                *dns++=host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++='\0';
}