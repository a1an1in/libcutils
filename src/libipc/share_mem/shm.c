#include <unistd.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/ipc.h>  
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/wait.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <errno.h>  
#include <sys/shm.h>  
  
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
}share_mem_t;

share_mem_t *shm_create()
{
    share_mem_t *shm;

    shm = (share_mem_t *)malloc(sizeof(share_mem_t));

    return shm;
}
int shm_set(share_mem_t *shm,char *key,int size,int type)
{
   strncpy(shm->shm_key,key,strlen(key));
   shm->size = size;
   shm->type = type;
   shm->oflags = O_RDWR | O_CREAT;
   shm->mmap_prot = PROT_READ | PROT_WRITE;
   shm->mmap_flag = MAP_SHARED;
   shm->mmap_off = 0;
   shm->shmflg = 0666|IPC_CREAT;
   shm->open_mode = 0644;

   return 1;
}
void *shm_smopen_get(share_mem_t *shm)
{
    int fd = shm_open(shm->shm_key, shm->oflags, shm->open_mode);  
    if (fd < 0) {  
        printf("shm_open failed, errormsg=%s errno=%d/n", strerror(errno), errno);    
        return NULL;  
    }  

    ftruncate(fd, shm->size);  

    shm->mem_addr = (void *)mmap(NULL, shm->size, shm->mmap_prot, shm->mmap_flag, fd, shm->mmap_off);     
    if (MAP_FAILED == shm->mem_addr) {  
        printf("mmap failed, errormsg=%s errno=%d/n", strerror(errno), errno);    
        return NULL;  
    }  

    return shm->mem_addr;
}
void *shm_open_get(share_mem_t *shm)
{
    int fd = open(shm->shm_key, shm->oflags, shm->open_mode);  
    if (fd < 0)  
    {  
        printf("ope failed, errormsg=%s errno=%d/n", strerror(errno), errno);  
        return NULL;  
    }  

    ftruncate(fd, shm->size);  

    shm->mem_addr = (void *)mmap(NULL, shm->size, shm->mmap_prot, shm->mmap_flag, fd, shm->mmap_off);     
    if (MAP_FAILED == shm->mem_addr) {  
        printf("mmap failed, errormsg=%s errno=%d/n", strerror(errno), errno);    
        return NULL;  
    }  

    return shm->mem_addr;
}
void *shm_shmget_get(share_mem_t *shm)
{
    int shmid;    
    struct shmid_ds buff;  

    shmid = shmget(ftok(shm->shm_key, 0), shm->size, shm->shmflg);  
    if (shmid < 0) {  
        printf("shmget failed, errormsg=%s errno=%d/n", strerror(errno), errno);  
        return NULL;  
    }  

    shm->mem_addr = (void *)shmat(shmid, NULL, 0);  
    if ((void *) -1 == shm->mem_addr)  
    {  
        printf("shmat failed, errormsg=%s errno=%d/n", strerror(errno), errno);  
        return NULL;  
    }  

    shmctl(shmid, IPC_STAT, &buff);  

    return  shm->mem_addr;
}
int shm_init(share_mem_t *shm)
{
    int ret = -1;
    if(shm->type == SHMOPEN){
        shm_smopen_get(shm);
    } else if(shm->type == OPEN) {
        shm_open_get(shm);
    } else if( shm->type == SHMGET) {
        shm_shmget_get(shm);
    } else{
    }

   return ret;
}

/*
 *int main()
 *{
 *}
 */
