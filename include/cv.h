#ifndef _MINIX_CV_H
#define _MINIX_CV_H

#include <minix/endpoint.h>

int get_cs_endpt(endpoint_t*);
int cs_lock(int);
int cs_unlock(int);
int cs_wait(int, int);
int cs_broadcast(int);

#endif /* _MINIX_CV_H */
