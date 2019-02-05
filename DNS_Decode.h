#ifndef DNS_DECODE_H
#define DNS_DECODE_H

//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

#include "CyoDecode.h" //encode into Base32

char* Decode(char*); 
char* dns_unsplit(char* encoded); //Used in Decode
char* ReadName(char*, char*, int*); //This will convert 3www6google3com to www.google.com

#endif