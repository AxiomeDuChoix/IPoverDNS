//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

#include "DNS_Decode.h"

#define MAX_SZ 32768

/*
int main(int argc, char* argv[])
{
    char msg[MAX_SZ];

	printf("Enter the msg to decode: ");
	fgets(msg, MAX_SZ, stdin);

    // Remove trailing newline, if there is.
    if ((strlen(msg) > 0) && (msg[strlen(msg)-1] == '\n'))
    {msg[strlen(msg)-1] = '\0';}

	char* decoded = DNS_Unsplit(msg);
	free(decoded);
	return 0;
}
*/

char* DNS_Unsplit(char* msg)
{
	
	printf("Original = '%s'\n", msg);

	/* Delete all the dots */

	char* unsplited = (char*) malloc(strlen(msg));
    int comp_uns = 0;

    for (int j=0; j<strlen(msg); j++)
    {
        if (msg[j] != '.') {
            unsplited[comp_uns] = msg[j];
            comp_uns ++;
        }
    }

    printf("Unsplited = '%s'\n", unsplited);

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
char* ReadName(char* reader, char* buffer)
{
    char *name = NULL;

    unsigned int p = 0, jumped = 0, offset;
    int i, j;
 
    name = (char*) malloc(MAX_SZ);
 
    name[0] = '\0';
 
    //read the names in 3www6google3com format
    while(*reader != 0)
    {
        if(*reader >= 192)
        {
            offset = (*reader) *256 + *(reader + 1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        }
        else
        {
            name[p++] = *reader;
        }
 
        reader = reader+1;
    }
 
    name[p]='\0'; //string complete
 
    //now convert 3www6google3com0 to www.google.com
    for(i=0; i<(int)strlen((const char*)name); i++) 
    {
        p=name[i];
        for(j=0; j<(int)p; j++)
        {
            name[i] = name[i+1];
            i++;
        }
        name[i]='.';
    }
    name[i-1] = '\0'; //remove the last dot
    return name;
}