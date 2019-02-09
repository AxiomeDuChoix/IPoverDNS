//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc

#include "DNS_Query.h"

/* Attention! After usage, free memory of:
 * dnspacket & dnspacket->header & dnspacket->question & dnspacket->record
 */
struct DNS_PACKET* new_from_binary(char* buf, int qname_len);