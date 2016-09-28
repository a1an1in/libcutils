#include <unistd.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
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


int sysv_sem_init(semaphore_t *sem)
{
    union semun sem_union;  

    sem->sysv_sem_id = semget(sem->sysv_key, sem->sysv_nsems, sem->sysv_semflg);  
  
    if(sem->host_flag == 1){
        sem_union.val = 1;  
        if(semctl(sem->sysv_sem_id, 0, SETVAL, sem_union) == -1)  
            return -1;  
    }
    return 1;  
}
  
static int sysv_sem_del(semaphore_t *sem)  
{  
    union semun sem_union;  
  
    if(semctl(sem->sysv_sem_id, 0, IPC_RMID, sem_union) == -1)  
        fprintf(stderr, "Failed to delete semaphore\n");  
    return 0;
}  
  
static int sysv_sem_p(semaphore_t *sem)  
{  
    //对信号量做减1操作，即等待P（sv）  
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = -1;//P()  
    sem_b.sem_flg = SEM_UNDO;  
    if(semop(sem->sysv_sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "semaphore_p failed\n");  
        return 0;  
    }  
    return 1;  
}  
  
static int sysv_sem_v(semaphore_t *sem)  
{  
    //这是一个释放操作，它使信号量变为可用，即发送信号V（sv）  
    struct sembuf sem_b;  
    sem_b.sem_num = 0;  
    sem_b.sem_op = 1;//V()  
    sem_b.sem_flg = SEM_UNDO;  
    if(semop(sem->sysv_sem_id, &sem_b, 1) == -1)  
    {  
        fprintf(stderr, "semaphore_v failed\n");  
        return 0;  
    }  
    return 1;  
}  

static semaphore_ops_t sysv_sem_ops = {
    .init = sysv_sem_init,
    .del  = sysv_sem_del,
    .p    = sysv_sem_p,
    .v    = sysv_sem_v,
};

semaphore_t *semaphore_create()
{
    semaphore_t *sem;

    sem = (semaphore_t *)malloc(sizeof(semaphore_t));

    return sem;
}
int semaphore_set(semaphore_t *sem,int type,int host_flag)
{
    sem->type = type;
    sem->sysv_nsems = 1;
    sem->sysv_semflg = 0666 | IPC_CREAT;
    sem->sysv_key = (key_t)1234;
    sem->host_flag = host_flag;

    return 1;
}
int semaphore_init(semaphore_t *sem)
{
    if(sem->type == SYSV_SEM){
        sem->sem_ops = &sysv_sem_ops;
    } else if(sem->type == POSIX_NONAME_SEM){
    } else if(sem->type == POSIX_NAME_SEM){
    } else {
    }

    sem->sem_ops->init(sem);
}
int semaphore_del(semaphore_t *sem)  
{
    sem->sem_ops->del(sem);
}
int semaphore_p(semaphore_t *sem)  
{
    sem->sem_ops->p(sem);
}
int semaphore_v(semaphore_t *sem)  
{
    sem->sem_ops->v(sem);
}

/*
 *int main()
 *{
 *}
 */
