/**
 * @file sem_sysv.c
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
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  
#include <libipc/semaphore/sem.h>
#include <libdbg/debug.h>

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
  
    if(sem->host_flag == 1)
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
        dbg_str(DBG_WARNNING,"semaphore_p failed");  
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
        dbg_str(DBG_WARNNING,"semaphore_v failed");  
        return 0;  
    }  
    return 1;  
}  

semaphore_ops_t sysv_sem_ops = {
    .init = sysv_sem_init,
    .del  = sysv_sem_del,
    .p    = sysv_sem_p,
    .v    = sysv_sem_v,
};

