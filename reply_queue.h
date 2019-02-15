// A C program to demonstrate linked list based implementation of queue 
#include <stdlib.h> 
#include <stdio.h> 
  
struct REPLY
{
    int key;
    char* msg;
};
// A linked list (LL) node to store a queue entry 
struct RNode 
{ 
    struct REPLY req; 
    struct RNode *next; 
}; 
  
// The queue, front stores the front node of LL and rear stores ths 
// last node of LL 
struct RQueue 
{ 
    struct RNode *front, *rear; 
}; 
  
// A utility function to create a new linked list node. 
struct RNode* newNode(struct REPLY reply) 
{ 
    struct RNode *temp = (struct RNode*)malloc(sizeof(struct RNode)); 
    temp->req = reply; 
    temp->next = NULL; 
    return temp;  
} 
  
// A utility function to create an empty queue 
struct RQueue *createQueue() 
{ 
    struct RQueue *q = (struct RQueue*)malloc(sizeof(struct RQueue)); 
    q->front = q->rear = NULL; 
    return q; 
} 
  
// The function to add a key k to q 
void enQueue(struct RQueue *q, struct REPLY reply) 
{ 
    // Create a new LL node 
    struct RNode *temp = newNode(reply); 
  
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
  
// Function to remove a reply from a given queue q 
struct RNode *deQueue(struct RQueue *q) 
{ 
    // If queue is empty, return NULL. 
    if (q->front == NULL) 
       return NULL; 

    // Store previous front and move front one node ahead 
    struct RNode *temp = q->front; 
    q->front = q->front->next; 
  
    // If front becomes NULL, then change rear also as NULL 
    if (q->front == NULL) 
       q->rear = NULL; 
    return temp; 
} 
  
// Peek function:


struct RNode *peek(struct RQueue *q)
{
    // If queue is empty, return NULL. 
    if (q->front == NULL) 
        return NULL; 
    struct RNode *temp = q->front;
    return temp;
}

int isEmpty(struct RQueue *q)
{
    int resu = (q->front == NULL);
    return resu;
}

int length(struct RQueue *q){
    int resu = 0;
    struct RNode *current = q->front;
    while (current!=NULL){
        resu++;
        current = current->next;
    }
    return resu;
}







