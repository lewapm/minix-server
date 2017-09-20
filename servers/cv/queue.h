#ifndef _QUEUE_H
#define _QUEUE_H

#include <minix/endpoint.h>
#include <sys/types.h>

struct Node{
	endpoint_t id;
	struct Node* next;
	struct Node* prev;
};

struct Queue{
	struct Node* head;
	struct Node* tail;
};

struct Queue* create();
endpoint_t front(struct Queue* q);
void pop(struct Queue* q);
int empty(struct Queue* q);
void insert(struct Queue* q, endpoint_t id);
int delete_el(struct Queue* q, endpoint_t idx);
void clean(struct Queue* q);
void print(struct Queue* q);

#endif
