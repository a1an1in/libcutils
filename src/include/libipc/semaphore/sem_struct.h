#ifndef __SEM_STRUCT_H__
#define __SEM_STRUCT_H__
#include <sys/sem.h>  
union semun  
{  
    int val;  
    struct semid_ds *buf;  
    unsigned short *arry;  
};  
enum sem_type_e{
    SYSV_SEM = 0, 
    POSIX_NONAME_SEM,
    POSIX_NAME_SEM,
};
  
struct semaphore_ops_s;

typedef struct semaphore_s{
    unsigned char type;
    int sem_id;
    key_t key;
    int sysv_nsems;
    int sysv_semflg;
    key_t sysv_key;
    int sysv_sem_id;
    int host_flag;
    struct semaphore_ops_s *sem_ops;
}semaphore_t;

typedef struct semaphore_ops_s{
    int (*init)(semaphore_t *sem);
    int (*del)(semaphore_t *sem);
    int (*p)(semaphore_t *sem);  
    int (*v)(semaphore_t *sem);  
}semaphore_ops_t;


semaphore_t *semaphore_create();
int semaphore_set(semaphore_t *sem,int type,int host_flag);
int semaphore_init(semaphore_t *sem);
int semaphore_del(semaphore_t *sem);
int semaphore_p(semaphore_t *sem);
int semaphore_v(semaphore_t *sem);
#endif
