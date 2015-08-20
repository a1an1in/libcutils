/*
 * =====================================================================================
 *
 *       Filename:  test_cds_alloc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/15/2015 11:18:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
/*  
 * Copyright (c) 2015-2010 alan lin <a1an1in@sina.com>
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
#include <stdlib.h>
#include "libcontainer/inc_files.h"

void test_ctr_alloc()
{
	allocator_t *allocator;
	void *p ,*p2,*p3;
	uint32_t size = 8;

	/*
	 *alloc_p->slab_max_num = SLAB_ARRAY_MAX_NUM;
	 *alloc_p->data_min_size = 8;
	 *alloc_p->mempool_capacity = MEM_POOL_MAX_SIZE;
	 */
	allocator = allocator_creator(ALLOCATOR_TYPE_CTR_MALLOC);
	allocator_ctr_init(allocator, 0, 0, 1024);
	/*
	 *allocator_cds_init(allocator,0,0,0);
	 */

	p = allocator_mem_alloc(allocator,7);
	dbg_str(DBG_CONTAINER_DETAIL,"alloc addr:%p",p);

	allocator_mem_free(allocator,p);

	p2 = allocator_mem_alloc(allocator,8);
	dbg_str(DBG_CONTAINER_DETAIL,"alloc addr:%p",p2);

	p3 = allocator_mem_alloc(allocator,200);
	dbg_str(DBG_CONTAINER_DETAIL,"alloc addr:%p",p3);

	dbg_str(DBG_CONTAINER_DETAIL,"inquire alloc info");
	allocator_mem_info(allocator);

	allocator_mem_free(allocator,p);
	allocator_mem_free(allocator,p2);
	allocator_mem_free(allocator,p3);

	dbg_str(DBG_CONTAINER_DETAIL,"batch alloc");
	int i;
	for(size = 8,i = 0; i< 20; i++,size += 8){
		p = allocator_mem_alloc(allocator,size);
	}
	dbg_str(DBG_CONTAINER_DETAIL,"inquire alloc info");
	allocator_mem_info(allocator);

	allocator_destroy(allocator);
	dbg_str(DBG_CONTAINER_DETAIL,"test cds alloc end");
}
