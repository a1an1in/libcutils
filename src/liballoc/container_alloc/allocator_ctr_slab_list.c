/*
 * =====================================================================================
 *
 *       Filename:  allocator_cds_slab_list.c 
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/15/2015 06:16:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include "libdbg/debug.h"
#include "liballoc/inc_files.h"

/*-----------------------------------------------------------------------------
 *  slab list operation
 *-----------------------------------------------------------------------------*/
void slab_init_head_list(struct list_head **hl_head)
{
	cds_slab_head_list_t *head_list;

	head_list = (cds_slab_head_list_t *)malloc(sizeof(cds_slab_head_list_t));
	if(head_list == NULL){
		dbg_str(DBG_ALLOC_ERROR,"malloc slab list_head_list");
		return;
	}
	pthread_rwlock_init(&head_list->head_lock,NULL);
	head_list->count = 0;
	INIT_LIST_HEAD(&head_list->list_head);
	*hl_head = &head_list->list_head;
}
void slab_release_head_list(struct list_head *hl_head)
{
	cds_slab_head_list_t *head_list;

	head_list = container_of(hl_head,cds_slab_head_list_t,list_head);
	pthread_rwlock_destroy(&head_list->head_lock);
	free(head_list);
}
void slab_attach_list(struct list_head *new_head,struct list_head *hl_head)
{
	cds_slab_head_list_t *head_list;
	head_list = container_of(hl_head,cds_slab_head_list_t,list_head);

	pthread_rwlock_wrlock(&head_list->head_lock);
	list_add(new_head,hl_head);
	head_list->count++;
	pthread_rwlock_unlock(&head_list->head_lock);

	dbg_str(DBG_ALLOC_DETAIL,"slab add head:%p, hl_head:%p",new_head,hl_head);
	dbg_str(DBG_ALLOC_DETAIL,"slab add heads next:%p, prev:%p",new_head->next,new_head->prev);
	cds_slab_t *slab_list;
	slab_list = container_of(new_head,cds_slab_t,list_head);
	if(slab_list->stat_flag == 0){
		dbg_str(DBG_ALLOC_IMPORTANT,"slab_size =%d,add free slab list,list count =%d",
				slab_list->slab_size,
				head_list->count);
	}else{
		dbg_str(DBG_ALLOC_IMPORTANT,"slab_size =%d,add used slab list,list count =%d",
				slab_list->slab_size,
				head_list->count);
	}
}
//del list,but not release it
void slab_detach_list(struct list_head *del_head,struct list_head *hl_head)
{
	cds_slab_head_list_t *head_list;

	head_list = container_of(hl_head,cds_slab_head_list_t,list_head);
	pthread_rwlock_wrlock(&head_list->head_lock);

	dbg_str(DBG_ALLOC_DETAIL,"slab del head:%p, hl_head:%p",del_head,hl_head);
	dbg_str(DBG_ALLOC_DETAIL,"slab del heads next:%p, prev:%p",del_head->next,del_head->prev);
	list_del(del_head);
	head_list->count--;
	pthread_rwlock_unlock(&head_list->head_lock);

	cds_slab_t *slab_list;
	slab_list = container_of(del_head,cds_slab_t,list_head);
	if(slab_list->stat_flag == 0){
		dbg_str(DBG_ALLOC_IMPORTANT,"slab_size =%d,del free slab list,list count =%d",
				slab_list->slab_size,
				head_list->count);
	}else{
		dbg_str(DBG_ALLOC_IMPORTANT,"slab_size =%d,del used slab list,list count =%d",
				slab_list->slab_size,
				head_list->count);
	}
}
cds_slab_t *slab_get_first_list(struct list_head *hl_head)
{
	cds_slab_head_list_t *head_list;
	cds_slab_t *slab_list;

	dbg_str(DBG_ALLOC_DETAIL,"slab_get_first_list");

	head_list = container_of(hl_head,cds_slab_head_list_t,list_head);
	pthread_rwlock_wrlock(&head_list->head_lock);
	if(hl_head->next != hl_head){
		head_list->count--;
		slab_list = container_of(hl_head->next,cds_slab_t,list_head);
		list_del(hl_head->next);
	}else{
		pthread_rwlock_unlock(&head_list->head_lock);
		return NULL;
	}
	pthread_rwlock_unlock(&head_list->head_lock);

	slab_list->stat_flag = 1;

	dbg_str(DBG_ALLOC_IMPORTANT,"slab_size =%d,del free slab list,list count =%d",
			slab_list->slab_size,
			head_list->count);
	dbg_str(DBG_ALLOC_DETAIL,"slab_get_first_list,mem_addr:%p",slab_list->mem_addr);

	return slab_list;
}
uint32_t slab_get_slab_index(allocator_t *alloc,uint32_t size)
{
	int data_min_size = alloc->priv.cds_alloc.data_min_size;

	return ((size + data_min_size - 1) / data_min_size) - 1;
}
cds_slab_t* slab_get_first_list_from_free_slabs(allocator_t *alloc,uint32_t size)
{
	struct list_head **free_slabs = alloc->priv.cds_alloc.free_slabs;
	uint32_t index;

	index = slab_get_slab_index(alloc,size);

	return slab_get_first_list(free_slabs[index]);
}
void slab_detach_list_from_used_slabs(allocator_t *alloc,struct list_head *del_head,uint32_t size)
{
	uint32_t index;
	struct list_head **used_slabs = alloc->priv.cds_alloc.used_slabs;

	index = slab_get_slab_index(alloc,size);

	return slab_detach_list(del_head,used_slabs[index]);
}
void slab_attach_list_to_used_slabs(allocator_t *alloc,struct list_head *new_head,uint32_t size)
{
	uint32_t index;
	struct list_head **used_slabs = alloc->priv.cds_alloc.used_slabs;

	index = slab_get_slab_index(alloc,size);

	return slab_attach_list(new_head,used_slabs[index]);
}
void slab_attach_list_to_free_slabs(allocator_t *alloc,struct list_head *new_head,uint32_t size)
{
	uint32_t index;
	struct list_head **free_slabs = alloc->priv.cds_alloc.free_slabs;

	index = slab_get_slab_index(alloc,size);

	return slab_attach_list(new_head,free_slabs[index]);
}
void slab_print_head_list(struct list_head *hl_head)
{
	cds_slab_head_list_t *head_list;
	head_list = container_of(hl_head,cds_slab_head_list_t,list_head);
	
	if(head_list->count)
		dbg_str(DBG_ALLOC_DETAIL,"slab head_list,count =%d",head_list->count);
}
void slab_print_list(cds_slab_t *slab_list)
{
	dbg_str(DBG_ALLOC_DETAIL,"slab info,size=%d,data_size =%d,slab_size=%d,slab_start:%p,slab_end:%p",
			slab_list->size,slab_list->data_size,
			slab_list->slab_size,
			slab_list,
			(uint8_t*)slab_list +  slab_list->size + sizeof(cds_slab_t));
}
void slab_print_list_for_each(struct list_head *hl_head)
{
	cds_slab_head_list_t *head_list;
	cds_slab_t *slab_list;
	struct list_head *pos,*n;

	slab_print_head_list(hl_head);

	head_list = container_of(hl_head,cds_slab_head_list_t,list_head);
	pthread_rwlock_rdlock(&head_list->head_lock);
	list_for_each_safe(pos, n, hl_head) {
		slab_list = container_of(pos,cds_slab_t,list_head);
		slab_print_list(slab_list);
	}
	pthread_rwlock_unlock(&head_list->head_lock);
}
