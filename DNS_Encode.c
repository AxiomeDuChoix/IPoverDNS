//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

#include "DNS_Encode.h"
#include "CyoEncode.h" //encode into Base32

#define MAX_SZ 32768
/*
int main(int argc, char* argv[])
{
    char msg[MAX_SZ];

	printf("Enter the msg to encode: ");
	fgets(msg, MAX_SZ, stdin);

	// Remove trailing newline, if there is.
	if ((strlen(msg) > 0) && (msg[strlen(msg)-1] == '\n'))
	{msg[strlen(msg)-1] = '\0';}

	char* result = Encode(msg);
	free(result);
	return 0;
}
*/

char* Encode (char* msg)
{
	size_t required = 0;
	char* encoded, *splited;

	/* Encode using Base32 */

	printf("Original = '%s'\n", msg);
	required = cyoBase32EncodeGetLength(strlen(msg));
	
	encoded = (char*) malloc(required);
	cyoBase32Encode(encoded, msg, strlen(msg)); 

	/* Change all "=" to "0" */

	for (char* p = encoded; p != NULL; p = strchr(p, '=')) {
	    *p = '0';
	}

	splited = (char*) malloc(required + required/63 + 1);
	splited = dns_split(encoded, required);

    free(encoded);
    printf("Splited = '%s'\n", splited);
    return splited;
}

/* Split the encoded Base32 message into sections of 63 bits (because in QNAME each word between the dots only has 64 bits)
 * The first word can only have 62 bits ("d" in front and "." behind)
 * The others can have 63 ("." behind)
 */

char* dns_split(char* encoded, size_t required){ 
	char* msg_encoded = (char*) malloc(required + required/63 + 1);
	int comp_enc = 0;
	int comp_msg = 0;
	int size_pack = 62;
	int len_encoded = strlen(encoded);

	while (1)
	{
		for (int j = 0; j < size_pack; j++)
		{
			if (comp_enc < len_encoded)
			{
				msg_encoded[comp_msg] = encoded[comp_enc];
				comp_enc++;
				comp_msg++;
			}
			else {return msg_encoded;}
		}
		if (comp_enc + 1 < len_encoded)
		{
			msg_encoded[comp_msg] = '.';
			comp_msg++;
		}
		else {return msg_encoded;}
		size_pack = 63;
	}
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