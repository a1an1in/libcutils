/*
 * =====================================================================================
 *
 *       Filename:  allocator_cds_alloc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/15/2015 09:51:08 AM
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
#include "libdbg/debug.h"
#include "liballoc/inc_files.h"

int cds_alloc_init(allocator_t *alloc)
{
	cds_alloc_t *alloc_p = &alloc->priv.cds_alloc;
	cds_mempool_t *mempool;
	struct list_head **free_slabs,**used_slabs;
	int slab_array_max_num;
	int i;

	dbg_str(DBG_ALLOC_DETAIL,"in cds_alloc_init");

	if(!alloc_p->slab_array_max_num) {
		alloc_p->slab_array_max_num = SLAB_ARRAY_MAX_NUM;
		dbg_str(DBG_ALLOC_DETAIL,"run at here");
	}
	if(!alloc_p->data_min_size){
		alloc_p->data_min_size = 8;
		dbg_str(DBG_ALLOC_DETAIL,"run at here");
	}
	if(!alloc_p->mempool_capacity){
		alloc_p->mempool_capacity = MEM_POOL_MAX_SIZE;
		dbg_str(DBG_ALLOC_DETAIL,"run at here");
	}

	dbg_str(DBG_ALLOC_DETAIL,"slab_num=%d,data_size=%d,cap=%d",alloc_p->slab_array_max_num,alloc_p->data_min_size,alloc_p->mempool_capacity);

	slab_array_max_num = alloc_p->slab_array_max_num;

	free_slabs = (struct list_head **)malloc(slab_array_max_num * 4);
	if(free_slabs == NULL){
		dbg_str(DBG_ERROR,"cds_alloc_init");
		return -1;
	}else{
		alloc_p->used_slabs = free_slabs;
	}
	used_slabs = (struct list_head **)malloc(slab_array_max_num * 4);
	if(used_slabs == NULL){
		dbg_str(DBG_ERROR,"cds_alloc_init");
		return -1;
	}else{
		alloc_p->free_slabs = used_slabs;
	}

	for(i = 0; i < slab_array_max_num; i++){
		slab_init_head_list(&used_slabs[i]);
		slab_init_head_list(&free_slabs[i]);
	}

	mempool_init_head_list(&alloc_p->pool);
	mempool = mempool_create_list(alloc);
	if(mempool == NULL){
		alloc_p->pool = NULL;
	}else{
		mempool_attach_list(&mempool->list_head,alloc_p->pool);
	}

	mempool_init_head_list(&alloc_p->empty_pool);

	dbg_str(DBG_ALLOC_DETAIL,"out cds_alloc_init");

	return 0;
}
void *cds_alloc_alloc(allocator_t *alloc,uint32_t size)
{
	cds_slab_t *slab_list;
	uint32_t index;

	index = slab_get_slab_index(alloc,size);
	dbg_str(DBG_ALLOC_DETAIL,"size=%d,index=%d,slab_array_max_num=%d",
			size,index,alloc->priv.cds_alloc.slab_array_max_num);
	if(size > alloc->priv.cds_alloc.mempool_capacity){
		dbg_str(DBG_ERROR,"apply szie excess mempool_capacity");
		return NULL;
	}
	if(index >= alloc->priv.cds_alloc.slab_array_max_num){
		dbg_str(DBG_ERROR,"apply size:%d too large,max size:%d,excess slab num,"
				"please assignd by sys malloc,or reconfig cds_alloc",size,20*8);
		return NULL;
	}else{
		dbg_str(DBG_ALLOC_DETAIL,"cds_alloc_alloc");
		slab_list = slab_get_first_list_from_free_slabs(alloc,size);
		if(slab_list == NULL){
			slab_list = mempool_alloc_slab_list(alloc,size);
		}
		slab_attach_list_to_used_slabs(alloc,&slab_list->list_head,size);
		return slab_list->mem_addr;
	}
}
void cds_alloc_free(allocator_t *alloc,void *addr)
{
	cds_slab_t *slab_list;
	struct list_head *new_head;
	struct list_head **free_slabs;
	uint32_t size;
	cds_alloc_t *alloc_p = &alloc->priv.cds_alloc;

	dbg_str(DBG_ALLOC_DETAIL,"cds_alloc_free");
	if(addr == NULL){
		dbg_str(DBG_ALLOC_DETAIL,"release addr is NULL");
		return;
	}
	slab_list = container_of(addr,cds_slab_t,data);
	if(slab_list->stat_flag == 0){
		dbg_str(DBG_WARNNING,"this addr has been released");
		return;
	}

	dbg_str(DBG_ALLOC_DETAIL,"release slab_list:%p",slab_list);
	dbg_str(DBG_ALLOC_DETAIL,"release mem_addr:%p,addr:%p",slab_list->mem_addr,addr);
	new_head = &slab_list->list_head;
	size = slab_list->size;
	free_slabs = alloc_p->free_slabs;

	slab_detach_list_from_used_slabs(alloc,
			new_head,//struct list_head *del_head,
			size);//uint32_t size);
	slab_list->stat_flag = 0;
	slab_attach_list_to_free_slabs(alloc,
			new_head,//struct list_head *new_head,
			size);//uint32_t size);
}
void cds_alloc_info(allocator_t *alloc)
{
	int i;
	int slab_array_max_num = alloc->priv.cds_alloc.slab_array_max_num;

	dbg_str(DBG_ALLOC_DETAIL,"query pool:");
	mempool_print_list_for_each(alloc->priv.cds_alloc.pool);

	dbg_str(DBG_ALLOC_DETAIL,"query empty_pool:");
	mempool_print_list_for_each(alloc->priv.cds_alloc.empty_pool);

	dbg_str(DBG_ALLOC_DETAIL,"query free_slabs:");
	for(i = 0; i < slab_array_max_num; i++){
		slab_print_list_for_each(alloc->priv.cds_alloc.free_slabs[i]);
	}

	dbg_str(DBG_ALLOC_DETAIL,"query used_slabs:");
	for(i = 0; i < slab_array_max_num; i++){
		slab_print_list_for_each(alloc->priv.cds_alloc.used_slabs[i]);
	}
}
void cds_alloc_destroy(allocator_t *alloc)
{
	int i;
	int slab_array_max_num = alloc->priv.cds_alloc.slab_array_max_num;
	struct list_head **free_slabs = alloc->priv.cds_alloc.free_slabs;
	struct list_head **used_slabs = alloc->priv.cds_alloc.used_slabs;

	dbg_str(DBG_ALLOC_DETAIL,"destroy pool");
	mempool_destroy_lists(alloc->priv.cds_alloc.pool);
	dbg_str(DBG_ALLOC_DETAIL,"destroy empty_pool");
	mempool_destroy_lists(alloc->priv.cds_alloc.empty_pool);

	dbg_str(DBG_ALLOC_DETAIL,"destroy free_slabs");
	for(i = 0; i < slab_array_max_num; i++){
		slab_release_head_list(free_slabs[i]);
		slab_release_head_list(used_slabs[i]);
	}
	free(used_slabs);
	free(free_slabs);
}

int allocator_cds_alloc_register(){
	allocator_module_t salloc = {
		.allocator_type = ALLOCATOR_TYPE_CDS_MALLOC,
		.alloc_ops = {
			.init    = cds_alloc_init,
			.alloc   = cds_alloc_alloc,
			.free    = cds_alloc_free,
			.destroy = cds_alloc_destroy,
			.info    = cds_alloc_info
		}
	};
	memcpy(&allocator_modules[ALLOCATOR_TYPE_CDS_MALLOC],&salloc,sizeof(allocator_module_t));
	return 0;
}
