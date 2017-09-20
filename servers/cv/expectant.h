#ifndef _EXPECTANT_H
#define _EXPECTANT_H

#include <minix/endpoint.h>
#include <sys/types.h>

struct NodeE{
	endpoint_t proc_nr;
	int mutex_id;
	struct NodeE* next;
	struct NodeE* prev;
};

struct Expectant{
	struct NodeE* head;
	struct NodeE* tail;
};

struct Expectant* createE();
endpoint_t front_proc_nr(struct Expectant* q);
int front_mutex_id(struct Expectant* q);
void popE(struct Expectant* q);
int emptyE(struct Expectant* q);
void insertE(struct Expectant* q, int mutex_id, endpoint_t proc_nr);
int delete_elE(struct Expectant* q, endpoint_t proc_nr);
void cleanE(struct Expectant* q);
void printE(struct Expectant* q);

#endif
