/*
 * =====================================================================================
 *
 *       Filename:  container.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/1/2015 
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
/*  
 * Copyright (c) 2015-2020 alan lin <a1an1in@sina.com>
 *  
 *  
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
#include <stdio.h>
#include <string.h>
#include "libcontainer/inc_files.h"

container_module_t container_modules[CONTAINER_TYPE_MAX_NUM];


container_t *container_creator(uint32_t container_type,allocator_t *allocator,uint8_t lock_type)
{
	container_t *p;

	p = (container_t *)allocator_mem_alloc(allocator,sizeof(container_t));
	if(p == NULL){
		goto err;
	}

	dbg_str(DBG_CONTAINER_DETAIL,"container_t addr:%p,sizeof_container_struct:%d",p,sizeof(container_t));
	p->container_type = container_type;
	p->allocator = allocator;
	p->c_ops_p = &container_modules[container_type].c_ops;
	p->it_ops_p = &container_modules[container_type].it_ops;
	/*
	 *pthread_rwlock_init(&p->head_lock,NULL);   
	 */
	sync_lock_init(&p->head_lock,lock_type);

	if(p->c_ops_p == NULL || p->it_ops_p == NULL){
		allocator_mem_free(p->allocator,p);
		p = NULL;
	}
err:
	return p;
}
