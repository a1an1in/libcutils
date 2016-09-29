/**
 * @file sem.c
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

    return sem->sem_ops->init(sem);
}
int semaphore_del(semaphore_t *sem)  
{
    return sem->sem_ops->del(sem);
}
int semaphore_p(semaphore_t *sem)  
{
    return sem->sem_ops->p(sem);
}
int semaphore_v(semaphore_t *sem)  
{
    return sem->sem_ops->v(sem);
}

