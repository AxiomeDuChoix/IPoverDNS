//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

#include "DNS_Query.h"
#include "Binary_from_New.h"

#define MAX_SZ 32768

/* Attention! After usage, free memory of buf */
char* binary_from_new(void* dnspacket_void)
	{
		struct DNS_PACKET *dnspacket = (struct DNS_PACKET*) dnspacket_void;
		
		char buf[MAX_SZ*2];
		char *pointer = NULL;
		pointer = buf;
		
		memcpy(pointer, dnspacket->header, sizeof(struct DNS_HEADER));
	
		pointer += sizeof(struct DNS_HEADER);
		memcpy(pointer, dnspacket->question, sizeof(struct QUESTION));
	
		pointer += sizeof(struct QUESTION);
		memcpy(record, dnspacket->record, sizeof(struct RES_RECORD));
	
		pointer += sizeof(struct RES_RECORD);
		pointer = '\0';
	
		/*
		char *buf = (char*) malloc(MAX_SZ*2);
		
		struct DNS_HEADER *header = (struct DNS_HEADER*) &buf;
		header = dnspacket->header;
		
		char *qname = (char*) &buf[sizeof(struct DNS_HEADER)];
		qname = dnspacket->qname;
		
		struct QUESTION *question = (struct QUESTION*) &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1)];
		question = dnspacket->question;
		
		struct RES_RECORD *record = (struct RES_RECORD*) &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION)];
		record = dnspacket->record;
		*/
		return buf;
	}
