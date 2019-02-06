//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

#include "DNS_Decode.h"
#include "CyoDecode.h" //encode into Base32

#define MAX_SZ 2048
int main(int argc, char* argv[])
{
    char msg[MAX_SZ];

	printf("Enter the msg to decode: ");
	fgets(msg, MAX_SZ, stdin);

    /* Remove trailing newline, if there is. */
    if ((strlen(msg) > 0) && (msg[strlen(msg)-1] == '\n'))
    {msg[strlen(msg)-1] = '\0';}

	char* decoded = Decode(msg);
	free(decoded);
	return 0;
}

char* Decode(char* msg)
{
	
	printf("Original = '%s'\n", msg);

	/* Delete all the dots */

	char* unsplited = dns_unsplit(msg);

	/* Change all "0" to "=" */

	for (char* p = unsplited; p != NULL; p = strchr(p, '0')) {
	    *p = '=';
	}

    printf("Unsplited = '%s'\n", unsplited);
	
	/* Decode using Base32 */

    size_t required = cyoBase32DecodeGetLength(strlen(unsplited));
    
    char* decoded = (char*) malloc(required);
    if (decoded == NULL)
    {
        printf("*** ERROR: Unable to allocate buffer for decoding ***\n");
        return NULL;
    }
    cyoBase32Decode(decoded, unsplited, strlen(unsplited));

    printf("Decoded = '%s'\n", decoded);
    free(unsplited);

    return decoded;
}

/* 
 *
 */

char* dns_unsplit(char* encoded){ 

	char* unsplited = (char*) malloc(strlen(encoded));
	int comp_uns = 0;

	for (int j=0; j<strlen(encoded); j++)
	{
		if (encoded[j] != '.') {
			unsplited[comp_uns] = encoded[j];
			comp_uns ++;
		}
	}
	return unsplited;
}

/*
The following is modified from :
https://gist.github.com/fffaraz/9d9170b57791c28ccda9255b48315168
which orginally performs a DNS Query from input hostname

Author : Silver Moon (m00n.silv3r@gmail.com)
Dated : 29/4/2009
*/

/*
 * This will convert 3www6google3com to www.google.com 
 * */

char* ReadName(char* reader, char* buffer, int* count)
{
    char *name;

    unsigned int p=0,jumped=0,offset;
    int i , j;
 
    *count = 1;
    name = (char*)malloc(256);
 
    name[0]='\0';
 
    //read the names in 3www6google3com format
    while(*reader!=0)
    {
        if(*reader>=192)
        {
            offset = (*reader)*256 + *(reader+1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        }
        else
        {
            name[p++]=*reader;
        }
 
        reader = reader+1;
 
        if(jumped==0)
        {
            *count = *count + 1; //if we havent jumped to another location then we can count up
        }
    }
 
    name[p]='\0'; //string complete
    if(jumped==1)
    {
        *count = *count + 1; //number of steps we actually moved forward in the packet
    }
 
    //now convert 3www6google3com0 to www.google.com
    for(i=0;i<(int)strlen((const char*)name);i++) 
    {
        p=name[i];
        for(j=0;j<(int)p;j++) 
        {
            name[i]=name[i+1];
            i=i+1;
        }
        name[i]='.';
    }
    name[i-1]='\0'; //remove the last dot
    return name;
}