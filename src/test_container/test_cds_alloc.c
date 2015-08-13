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
#include <stdio.h>
#include <stdlib.h>
#include "libcontainer/inc_files.h"

void test_cds_alloc()
{
	allocator_t *allocator;
	void *p ,*p2,*p3;
	uint32_t size = 8;

	/*
	 *alloc_p->slab_max_num = SLAB_ARRAY_MAX_NUM;
	 *alloc_p->data_min_size = 8;
	 *alloc_p->mempool_capacity = MEM_POOL_MAX_SIZE;
	 */
	allocator = allocator_creator(ALLOCATOR_TYPE_CDS_MALLOC);
	allocator_cds_init(allocator, 0, 0, 1024);
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
