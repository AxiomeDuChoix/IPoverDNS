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
		struct DNS_PACKET *dnspacket = NULL;
		dnspacket = (struct DNS_PACKET*) malloc(sizeof(struct DNS_PACKET));
	    dnspacket->header = (struct DNS_HEADER*) malloc(sizeof(struct DNS_HEADER));
	    dnspacket->qname = (char *) malloc(qname_len);
	    dnspacket->question = (struct QUESTION*) malloc(sizeof(struct QUESTION));
			
		struct DNS_HEADER *header = NULL;
		header = dnspacket->header;
		memcpy(header, buf, sizeof(struct DNS_HEADER));
		dnspacket->header = header;
    	
    	char* pointer = buf + sizeof(struct DNS_HEADER);
    	char* qname = NULL;
    	qname = dnspacket->qname;
    	qname = &pointer[qname_len];

    	pointer += qname_len;
    	struct QUESTION* question = NULL;
    	question = dnspacket->question;
    	memcpy(question, pointer, sizeof(struct QUESTION));
    	
    	pointer += sizeof(struct Question);
		struct RES_RECORD *record = NULL;
		record = dnspacket->record;
		memcpy(question, pointer, sizeof(struct QUESTION));

		return dnspacket;
	}