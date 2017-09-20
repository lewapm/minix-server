#include <errno.h>
#include <stdio.h>
#include <cv.h>
#include <lib.h>
#include <unistd.h>
#include <minix/rs.h>

#define OK 0

int get_cv_endpt(endpoint_t *pt){
	return minix_rs_lookup("cv", pt);
}

int cs_lock(int mutex_id){
	message m;
	endpoint_t cv_pt;
	int result = get_cv_endpt(&cv_pt);
	int r;
	m.m1_i1 = mutex_id;
	while((r = _syscall(cv_pt, 1, &m)) < 0){
		if(errno == EINTR){
			m.m1_i1 = mutex_id;
		}
		else{
			return -1;
		}	
	}
	return 0;
}

int cs_unlock(int mutex_id){
	message m;
	endpoint_t cv_pt;
	int result = get_cv_endpt(&cv_pt);
	if(result != OK){
		printf("Jest problem z get_cs_endpt cs_unlock\n");
	}
	m.m1_i1 = mutex_id;
	return (_syscall(cv_pt, 2, &m)); 	
}

int cs_wait(int cond_var_id, int mutex_id){
	message m;
	endpoint_t cv_pt;
	int result = get_cv_endpt(&cv_pt);
	int r;
	m.m1_i1 = cond_var_id;
	m.m1_i2 = mutex_id;	
	r = _syscall(cv_pt, 3, &m);
	if(r == -1){
		if(errno == EINTR){
			r = cs_lock(mutex_id);
			return r;
		}
		else{
			return -1;
		}
	}
	return r;
}

int cs_broadcast(int cond_var_id){
	message m;
	endpoint_t cv_pt;
	int result = get_cv_endpt(&cv_pt);
	if(result != OK){
		printf("Jest problem z get_cv_endpt cv_unlock\n");
	}
	m.m1_i1 = cond_var_id;
	return (_syscall(cv_pt, 4, &m));
}
