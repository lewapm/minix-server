#include "expectant.h"
#include <stdlib.h>

struct Expectant* createE(){
	struct Expectant* q = malloc(sizeof(struct Expectant));
	q->head = malloc(sizeof(struct NodeE));
	q->tail = malloc(sizeof(struct NodeE));
	q->head->mutex_id = q->tail->mutex_id = -1;
	q->head->proc_nr = q->tail->proc_nr = -1;
	q->head->next = q->head->prev = q->tail;
	q->tail->next = q->tail->prev = q->head; 
	return q;
}

endpoint_t front_proc_nr(struct Expectant* q){
	return q->head->next->proc_nr;
}

int front_mutex_id(struct Expectant* q){
	return q->head->next->mutex_id;
}

void popE(struct Expectant* q){
	if(emptyE(q) == 0){
		struct NodeE* tmp = q->head->next;
		tmp->next->prev = q->head;
		q->head->next = tmp->next;
		free(tmp);
	}
}

int emptyE(struct Expectant* q){
	if(q->head->next == q->tail)
		return 1;
	return 0; //not empty
}

void insertE(struct Expectant* q, int mutex_id, endpoint_t proc_nr){
	struct NodeE* tmp = malloc(sizeof(struct NodeE));
	tmp->mutex_id = mutex_id;
	tmp->proc_nr = proc_nr;
	tmp->next = q->tail;
	tmp->prev = q->tail->prev;
	q->tail->prev->next = tmp;
	q->tail->prev = tmp;
}

int delete_elE(struct Expectant* q, endpoint_t proc_nr){
	struct NodeE* tmp = q->head;
	while(tmp->proc_nr != proc_nr){
		tmp = tmp->next;
		if(tmp == q->tail){
			return 0;
		}
	}
	tmp->prev->next = tmp->next;
	tmp->next->prev = tmp->prev;
	free(tmp);
	return 1;
}

void cleanE(struct Expectant* q){
	struct NodeE* tmp;
	while(q->head->next != q->tail){
		tmp = q->head->next;
		q->head->next = tmp->next;
		tmp->next->prev = q->head;
		free(tmp);
	}
	free(q->head);
	free(q->tail);
	free(q);
}

void printE(struct Expectant* q){
	struct NodeE* tmp = q->head;
	while(tmp != q->tail){
		printf("print Expectant: %d %d\n", tmp->mutex_id, tmp->proc_nr);
		tmp = tmp->next;
	}
	printf("koniec print Expectant: %d %d\n", tmp->mutex_id, tmp->proc_nr);	
}
