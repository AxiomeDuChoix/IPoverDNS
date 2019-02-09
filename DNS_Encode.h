#ifndef DNS_SPLIT_H
#define DNS_SPLIT_H

//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

char* DNS_Split(char* encoded);
void ChangetoDnsNameFormat(char*, char*); //This will convert www.google.com to 3www6google3com

#endif