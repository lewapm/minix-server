#include "cv.h"
#include "queue.h"
#include "expectant.h"

endpoint_t who_e;
int call_type;

/* SEF functions and variables. */
static void sef_local_startup(void);
static int sef_cb_init_fresh(int type, sef_init_info_t *info);
static void sef_cb_signal_handler(int signo);

int used[1025];
int mutex_id[1025];
struct Queue* mutex_que[1025]={NULL};
int usedvar[VARSIZE];
int cond_var[VARSIZE];
struct Expectant* cond_exp[VARSIZE]={NULL};


int main(int argc, char *argv[])
{
	message m;

	/* SEF local startup. */
	env_setargs(argc, argv);
	sef_local_startup();
	int licznik = 0;
	while (TRUE) {
		int r;
		if ((r = sef_receive(ANY, &m)) != OK)
			printf("sef_receive failed %d.\n", r);
		who_e = m.m_source;
		call_type = m.m_type;
		int result;
		switch(call_type){
			case CS_LOCK:
				result = do_cs_lock(&m);
				m.m_type = result;
				break;
			case CS_UNLOCK:
				result = do_cs_unlock(&m);
				m.m_type = result;
				break;
			case CS_WAIT:
				result = do_cs_wait(&m);
				m.m_type = result;
				break;
			case 4:
				result = do_cs_broadcast(&m);
				m.m_type = result;
				break;
			case PM_UNPAUSE:
				m.m_source = m.PM_PROC;
				who_e = m.PM_PROC;
				result = do_unpause(&m);
				
				if(result == OK){
					m.m_type = EINTR;
					result = EINTR;
				}
				else
					result = EDONTREPLY;
				break;
			case PM_DUMPCORE:
			case PM_EXIT:
				licznik++;
				m.m_source = m.PM_PROC;
				result = do_exit(&m);
				result = EDONTREPLY;
				m.m_source = who_e;
				break;
			default: 
				printf("CV unknown call type: %d from %d.\n", call_type, who_e);
				m.m_type = EINVAL;
				break;
		}
		if(result != EDONTREPLY){
			send(who_e, &m);
		}
	}

	/* no way to get here */
	return -1;
}

/*===========================================================================*
 *			       sef_local_startup			     *
 *===========================================================================*/
static void sef_local_startup()
{
  /* Register init callbacks. */
  sef_setcb_init_fresh(sef_cb_init_fresh);
  sef_setcb_init_restart(sef_cb_init_fresh);

  sef_setcb_signal_handler(sef_cb_signal_handler);

  /* Let SEF perform startup. */
  sef_startup();
}

/*===========================================================================*
 *		            sef_cb_init_fresh                                *
 *===========================================================================*/
static int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t *UNUSED(info))
{
  return(OK);
}

/*===========================================================================*
 *		            sef_cb_signal_handler                            *
 *===========================================================================*/
static void sef_cb_signal_handler(int signo)
{
  if (signo != SIGTERM) return;
}

int do_cs_lock(message *m)
{
	int mutex = m->m1_i1;
	int i = 0;
	int first = 1024;
	endpoint_t who = m->m_source;
	for(i = 0; i < 1024; ++i){
		if((used[i] == 1) && (mutex_id[i] == mutex)){
			struct Queue* q = mutex_que[i];
			if(front(q) == who){
				return EINVAL;
			}
			else{
				insert(mutex_que[i], who);
				return EDONTREPLY;
			}
		}
		else{
			if(used[i] == 0){
				if(i < first)
					first = i;
			}
		}
	}
	if(first < 1024){
		used[first] = 1;
		mutex_id[first] = mutex;
		mutex_que[first] = create();
		insert(mutex_que[first], who);
		return OK;
	}
	return EINVAL;
}

int do_cs_unlock(message *m){
	int mutex = m->m1_i1;
	endpoint_t who = m->m_source;
	int i = 0, result = 0;
	for(i = 0; i < 1024; ++i){
		if(used[i] == 1 && mutex_id[i] == mutex){
			if(front(mutex_que[i]) == who){
				pop(mutex_que[i]);
				if(empty(mutex_que[i]) == 0){
					message m1;
					m1.m_type = OK;
					result = sendnb(front(mutex_que[i]), &m1);
				}
				else{
					clean(mutex_que[i]);
					mutex_que[i] = NULL;
					used[i] = 0;
				}
				return OK;
			}
			else{
				return EPERM;
			}
		}
	}
	return EPERM;
}

int do_cs_wait(message* m){
	int cond = m->m1_i1;
	int mutex = m->m1_i2;
	int i = 0, result;
	message m1;
	m1.m1_i1 = mutex;
	m1.m_source = m->m_source;
	result = do_cs_unlock(&m1);
	if(result != OK){
		return EINVAL;
	}
	else{
		int first = VARSIZE+1;
		for(i = 0; i < VARSIZE; ++i){
			if(usedvar[i] == 1 && cond_var[i] == cond){
				insertE(cond_exp[i], mutex, m->m_source);
				return EDONTREPLY;
			}
			else{
				if(usedvar[i] == 0 && first > i)
					first = i;
			}
		}
		if(first < VARSIZE){
			usedvar[first] = 1;
			cond_var[first] = cond;
			cond_exp[first] = createE();
			insertE(cond_exp[first], mutex, m->m_source);
			return EDONTREPLY;
		}
		else{
			return EINVAL;
		}

	}
}

int do_cs_broadcast(message* m){
	int cond = m->m1_i1;
	int i = 0, y, result;
	endpoint_t x;
	for(i = 0; i < VARSIZE; ++i){
		if(usedvar[i] == 1 && cond_var[i] == cond){
			struct Expectant* q = cond_exp[i];
			while(emptyE(q) == 0){
				x = front_proc_nr(q);
				y = front_mutex_id(q);
				popE(q);
				message m1;
				m1.m1_i1 = y;
				m1.m_source = x;
				result = do_cs_lock(&m1);
				if(result == OK){
					m1.m_type = OK;
					result = send(x, &m1);
				}
			}
			usedvar[i] = 0;
			cleanE(cond_exp[i]);
			cond_exp[i] = NULL;
			return OK;
		}
	}
	return OK;
}

int do_unpause(message * m){
	endpoint_t who = m->m_source;
	int i = 0, l=0;
	for(i = 0; i < 1024; ++i){
		if(used[i] == 1){
			if(front(mutex_que[i]) == who){
				l++;
				continue;
			} 
			else{
				int r = delete_el(mutex_que[i], who); 
				if(r == 1)
					l++;
				if(r == 1 && empty(mutex_que[i]) == 1){
					clean(mutex_que[i]);
					mutex_que[i] = NULL;
					used[i] = 0;
				}
			}
		}
	}
	int t = 0;
	for(i = 0; i < VARSIZE; ++i){
		if(usedvar[i] == 1){
			t = delete_elE(cond_exp[i], who);
			if(t == 1){
				if(emptyE(cond_exp[i]) == 1){
					usedvar[i] = 0;
					cleanE(cond_exp[i]);
					cond_exp[i] = NULL;
				}
				return OK;
			}
		}
	}
	if(l > 0)
		return OK;
	return EINVAL;
}

int do_exit(message *m){
	endpoint_t who = m->m_source;
	int i = 0;
	for(i = 0; i < 1024; ++i){
		if(used[i] == 1){
			if(front(mutex_que[i]) == who){
				message m1;
				m1.m1_i1 = mutex_id[i];
				m1.m_source = who;
				do_cs_unlock(&m1);
			} 
			else{
				int r = delete_el(mutex_que[i], who);
				if(r == 1 && empty(mutex_que[i]) == 1){
					used[i] = 0;
					clean(mutex_que[i]);
					mutex_que[i] = NULL;
				}
			}
		}
	}
	for(i = 0; i < VARSIZE; ++i){
			
		if(usedvar[i] == 1){
			int r = delete_elE(cond_exp[i], who);
			if(r == 1){
				if(emptyE(cond_exp[i]) == 1){
					usedvar[i] = 0;
					cleanE(cond_exp[i]);
					cond_exp[i] = NULL;
				}
				break;
			}
			
		}
	}
	return OK;
}

