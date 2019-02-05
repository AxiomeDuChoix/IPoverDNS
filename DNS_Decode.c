//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

<<<<<<< HEAD
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
=======
#include "CyoDecode.h" //decode from Base32

char* Decode (unsigned char*);
char* dns_unsplit (char* encoded, size_t required); //Used in Encode
unsigned char* ReadName (unsigned char*, unsigned char*, int*); //This will convert 3www6google3com to www.google.com

int main(int argc, char* argv[])
{
    printf("Enter the msg to encode:");
    scanf("%s", msg);

    printf("\n%s", Decode(msg));
    return 0;
}

char* Decode (unsigned char* msg)
{
	size_t required = 0;
	char* encoded = NULL;
	int valid = 0;

	/* Encode using Base32 */

	printf("Original = '%s'\n", msg);
	required = cyoBase64EncodeGetLength(strlen(msg));
	encoded = (char*)malloc(required);
	if (encoded == NULL)
	{
	printf("*** ERROR: Unable to allocate buffer for encoding ***\n");
	goto exit;
	}
	cyoBase32Encode(encoded, msg, strlen(msg));

	/* Validate encoding */

	valid = cyoBase32Validate(encoded, strlen(encoded));
	if (valid < 0)
	{
	printf("*** ERROR: Encoding failure (error %d) ***\n", valid);
	goto exit;
	} 

	/* Change all "=" to "0" */
	for (char* p = encoded; p = strchr(p, '='); ++p) {
	    *p = '0';
	}

	return dns_split(encoded, required);

	printf("Encoded = '%s'\n", msg_encoded);
}

/* Split the encoded Base32 message into sections of 63 bits (because in QNAME each word between the dots only has 64 bits)
 * The first word can only have 62 bits ("d" in front and "." behind)
 * The others can have 63 ("." behind)
 */

char* dns_split(char* encoded, size_t required){ 
	char msg_encoded[required + required/63 + 1];
	int comp_enc = 0;
	int comp_msg = 0;
	int size_pack = 62;
	int len_encoded = strlen(encoded);

	while (1){
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
>>>>>>> b314e18594536042d83fb0c0060edfc8f8df9219
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
<<<<<<< HEAD
char* ReadName(char* reader, char* buffer, int* count)
{
    char *name;
=======
u_char* ReadName(unsigned char* reader, unsigned char* buffer, int* count)
{
    unsigned char *name;
>>>>>>> b314e18594536042d83fb0c0060edfc8f8df9219
    unsigned int p=0,jumped=0,offset;
    int i , j;
 
    *count = 1;
<<<<<<< HEAD
    name = (char*)malloc(256);
=======
    name = (unsigned char*)malloc(256);
>>>>>>> b314e18594536042d83fb0c0060edfc8f8df9219
 
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