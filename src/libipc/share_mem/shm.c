/**
 * @file shm.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 1
 * @date 2016-09-28
 */
/* Copyright (c) 2015-2020 alan lin <a1an1in@sina.com>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <unistd.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/ipc.h>  
#include <sys/shm.h>
#include <sys/mman.h>  
#include <sys/types.h>  
#include <sys/wait.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <errno.h>  
#include <libdbg/debug.h>
#include <libipc/share_mem/shm.h>

  
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
    dbg_str(DBG_DETAIL,"shm_mem_addr=%p",shm->mem_addr);

    semaphore_init(shm->sem);

    return shm->mem_addr;
}
int shm_smopen_del(share_mem_t *shm)
{
    int ret;

    ret = munmap(shm->mem_addr, shm->size);
    if(ret){
        dbg_str(DBG_DETAIL," munmap error");
    }
    ret = shm_unlink(shm->shm_key);
    if(ret){
        dbg_str(DBG_DETAIL,"shm_unlink error");
    }
    semaphore_del(shm->sem);

    return ret;
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
    semaphore_init(shm->sem);

    close(fd);
    return shm->mem_addr;
}
int shm_open_del(share_mem_t *shm)
{
    int ret;

    ret = munmap(shm->mem_addr, shm->size);
    if(ret){
        dbg_str(DBG_DETAIL," munmap error");
    }
    ret = shm_unlink(shm->shm_key);
    if(ret){
        dbg_str(DBG_DETAIL,"shm_unlink error");
    }
    semaphore_del(shm->sem);
    return ret;
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
    shm->shmid = shmid;

    semaphore_init(shm->sem);

    return  shm->mem_addr;
}
int shm_shmget_del(share_mem_t *shm)
{
    struct shmid_ds  buf;

    shmdt(shm->mem_addr);
    shmctl(shm->shmid,IPC_RMID,&buf);
    semaphore_del(shm->sem);

    return 0;
}

share_mem_t *shm_create()
{
    share_mem_t *shm;

    shm = (share_mem_t *)malloc(sizeof(share_mem_t));

    shm->sem = semaphore_create();

    return shm;
}
int shm_set(share_mem_t *shm,char *key,int size,int shm_type,int sem_type,int sem_host_flag)
{
   strncpy(shm->shm_key,key,strlen(key));
   shm->size = size;
   shm->type = shm_type;
   shm->oflags = O_RDWR | O_CREAT;
   shm->mmap_prot = PROT_READ | PROT_WRITE;
   shm->mmap_flag = MAP_SHARED;
   shm->mmap_off = 0;
   shm->shmflg = 0666|IPC_CREAT;
   shm->open_mode = 0644;

   semaphore_set(shm->sem,sem_type,sem_host_flag);

   return 1;
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
int shm_del(share_mem_t *shm) 
{
    int ret = -1;
    if(shm->type == SHMOPEN){
        shm_smopen_del(shm);
    } else if(shm->type == OPEN) {
        shm_open_del(shm);
    } else if( shm->type == SHMGET) {
        shm_shmget_del(shm);
    } else{
        dbg_str(DBG_WARNNING,"shm del,shm type error");
    }

   return ret;
}

void test_share_mem_write()
{
    share_mem_t *shm;
    char *key = "test_share_mem";
    int size = 1024;
    char *data = "hello world!";


    shm = shm_create();
    shm_set(shm,key,size,SHMOPEN,SYSV_SEM,1);
    shm_init(shm);

    semaphore_p(shm->sem);
    memcpy(shm->mem_addr,data,strlen(data));
    dbg_str(DBG_DETAIL,"read data from shmem:%s",shm->mem_addr);

    sleep(30);
    semaphore_v(shm->sem);
    sleep(60);
    shm_del(shm);

}
void test_share_mem_read()
{
    share_mem_t *shm;
    char *key = "test_share_mem";
    int size = 1024;


    shm = shm_create();
    shm_set(shm,key,size,SHMOPEN,SYSV_SEM,0);
    shm_init(shm);

    semaphore_p(shm->sem);
    dbg_str(DBG_DETAIL,"%s",shm->mem_addr);
    semaphore_v(shm->sem);

    sleep(60);
    shm_del(shm);

}
