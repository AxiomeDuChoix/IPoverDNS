#ifndef DNS_UNSPLIT_H
#define DNS_UNSPLIT_H

//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

char* DNS_Unsplit(char*);
char* ReadName(char*, char*); //This will convert 3www6google3com to www.google.com

#endif