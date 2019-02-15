// A C program to demonstrate linked list based implementation of queue 
#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "DNS_Query.h"

struct QUERY
{
    struct DNS_PACKET *rep;
    struct sockaddr_in *sender;
    time_t now;
};
// A linked list (LL) node to store a queue entry 
struct QNode 
{ 
    struct QUERY *req; 
    struct QNode *next; 
}; 
  
// The queue, front stores the front node of LL and rear stores ths 
// last node of LL 
struct QQueue 
{ 
    struct QNode *front, *rear; 
}; 
  
// A utility function to create a new linked list node. 
struct QNode* QnewNode(struct QUERY *request) 
{ 
    struct QNode *temp = (struct QNode*)malloc(sizeof(struct QNode)); 
    temp->req = request; 
    temp->next = NULL; 
    return temp;  
} 
  
// A utility function to create an empty queue 
struct QQueue *QcreateQueue() 
{ 
    struct QQueue *q = (struct QQueue*)malloc(sizeof(struct QQueue)); 
    q->front = q->rear = NULL; 
    return q; 
} 
  
// The function to add a key k to q 
void QenQueue(struct QQueue *q, struct QUERY *request) 
{ 
    // Create a new LL node 
    struct QNode *temp = QnewNode(request); 
  
    // If queue is empty, then new node is front and rear both 
    if (q->rear == NULL) 
    { 
       q->front = q->rear = temp; 
       return; 
    } 
  
    // Add the new node at the end of queue and change rear 
    q->rear->next = temp; 
    q->rear = temp; 
} 
  
// Function to remove a Request from a given queue q 
struct QNode *QdeQueue(struct QQueue *q) 
{ 
    // If queue is empty, return NULL. 
    if (q->front == NULL) 
       return NULL; 
  
    // Store previous front and move front one node ahead 
    struct QNode *temp = q->front; 
    q->front = q->front->next; 
  
    // If front becomes NULL, then change rear also as NULL 
    if (q->front == NULL) 
       q->rear = NULL; 
    return temp; 
} 
  
// Peek function:


struct QNode *Qpeek(struct QQueue *q)
{
    // If queue is empty, return NULL. 
    if (q->front == NULL) 
        return NULL; 
    struct QNode *temp = q->front;
    return temp;
}
int QisEmpty(struct QQueue *q)
{
    int resu = (q->front == NULL);
    return resu;
}
