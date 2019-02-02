//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

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
u_char* ReadName(unsigned char* reader, unsigned char* buffer, int* count)
{
    unsigned char *name;
    unsigned int p=0,jumped=0,offset;
    int i , j;
 
    *count = 1;
    name = (unsigned char*)malloc(256);
 
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