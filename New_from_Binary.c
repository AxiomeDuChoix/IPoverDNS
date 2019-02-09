//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

#include "DNS_Query.h"
#include "New_from_Binary.h"

#define MAX_SZ 32768

/* Attention! After usage, free memory of:
 * dnspacket & dnspacket->header & dnspacket->qname & dnspacket->question & dnspacket->record
 */
struct DNS_PACKET* new_from_binary(char* buf, int qname_len)
	{
		struct DNS_PACKET *dnspacket = (struct DNS_PACKET*) malloc(sizeof(struct DNS_PACKET));
		
		struct DNS_HEADER *header = (struct DNS_HEADER*) malloc(sizeof(struct DNS_HEADER));
		memcpy(header, buf, sizeof(struct DNS_HEADER));
		dnspacket->header = header;
    	
    	char* pointer = buf + sizeof(struct DNS_HEADER);
    	char* qname = (char*) malloc(qname_len);
    	qname = &pointer[qname_len];
    	dnspacket->qname = qname;

    	pointer += qname_len;
    	struct QUESTION* question = (struct QUESTION*) malloc(sizeof(struct QUESTION));
    	memcpy(question, pointer, sizeof(struct QUESTION));
    	dnspacket->question = question;
    	
		struct RES_RECORD *record = (struct RES_RECORD*) pointer;
		dnspacket->record = record;
		
		return dnspacket;
	}