#ifndef __SEM_H__
#define __SEM_H__
#include <sys/sem.h>  
#include <libcre/semaphore/sem_struct.h>
#include <libcre/semaphore/sem_sysv.h>

semaphore_t *semaphore_create();
int semaphore_set(semaphore_t *sem,int type,int host_flag);
int semaphore_init(semaphore_t *sem);
int semaphore_del(semaphore_t *sem);
int semaphore_p(semaphore_t *sem);
int semaphore_v(semaphore_t *sem);
#endif
