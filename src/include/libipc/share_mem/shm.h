#ifndef __SHM_H__
#define __SHM_H__
#include <libcre/semaphore/sem.h>

enum share_mem_type_e{  
    SHMOPEN = 0x01,  
    OPEN = 0x02,  
    SHMGET = 0x04,  
};  
  
typedef struct share_mem_s{
#define MAX_SHARE_MEM_KEY_LEN 100
    int type;
    char shm_key[MAX_SHARE_MEM_KEY_LEN];
#undef MAX_SHARE_MEM_KEY_LEN
    int size;
    void *mem_addr;
    int oflags;
    int mmap_prot;
    int mmap_flag;
    int mmap_off;
    int shmflg;
    int open_mode;
    struct share_mem_ops_s *shm_ops;
    semaphore_t *sem;
}share_mem_t;
typedef struct share_mem_ops_s{
    void *(*get)(share_mem_t *shm);
    int (*del)(share_mem_t *shm);
}share_mem_ops_t;

share_mem_t *shm_create();
int shm_set(share_mem_t *shm,char *key,int size,int type,int sem_type,int sem_host_flag);
int shm_init(share_mem_t *shm);
int shm_del(share_mem_t *shm);
void test_share_mem_write();
void test_share_mem_read();
#endif
