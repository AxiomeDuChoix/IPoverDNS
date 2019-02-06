#ifndef DNS_ENCODE_H
#define DNS_ENCODE_H

//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

#include "CyoEncode.h" //encode into Base32

char* Encode(char*);
char* dns_split(char* encoded, size_t required); //Used in Encode
void ChangetoDnsNameFormat(char*, char*); //This will convert www.google.com to 3www6google3com

#endif