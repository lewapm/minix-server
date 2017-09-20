#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
struct Queue* create(){
	struct Queue* q = malloc(sizeof(struct Queue));
	q->head = malloc(sizeof(struct Node));
	q->tail = malloc(sizeof(struct Node));
	q->head->id = q->tail->id = -1;
	q->head->next = q->head->prev = q->tail;
	q->tail->next = q->tail->prev = q->head; 
	return q;
}

endpoint_t front(struct Queue* q){
	return q->head->next->id;
}

void pop(struct Queue* q){
	if(empty(q) == 0){
		struct Node* tmp = q->head->next;
		tmp->next->prev = q->head;
		q->head->next = tmp->next;
		free(tmp);
	}
}

int empty(struct Queue* q){
	if(q->head->next == q->tail)
		return 1;
	return 0; //not empty
}

void insert(struct Queue* q, endpoint_t id){
	struct Node* tmp = malloc(sizeof(struct Node));
	tmp->id = id;
	tmp->next = q->tail;
	tmp->prev = q->tail->prev;
	q->tail->prev->next = tmp;
	q->tail->prev = tmp;
}

int delete_el(struct Queue* q, endpoint_t idx){
	struct Node* tmp = q->head;
	while(tmp->id != idx){
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

void clean(struct Queue* q){
	struct Node* tmp;
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

void print(struct Queue* q){
    struct Node* tmp = q->head;
    while(tmp != q->tail){
        printf("print Queue: %d ", tmp->id);
        tmp = tmp->next;
    }
	printf("koniec print Queue: %d\n", tmp->id);
}


