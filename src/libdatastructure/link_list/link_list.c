/*
 * =====================================================================================
 *
 *       Filename:  list_t.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "libdbg/debug.h"
#include "libdatastructure/link_list.h"
#include "libdatastructure/memory_management.h"


int llist_pos_init(list_pos_t *pos,struct list_head *lh,llist_t *llist)
{

	pos->llist = llist;
	pos->list_head_p = lh;
	return 0;
}

list_pos_t llist_begin(llist_t *llist)
{
	return llist->begin;
}

list_pos_t llist_end(llist_t *llist)
{
	return llist->end;
}
list_pos_t llist_pos_next(list_pos_t pos)
{
	list_pos_t ret;

	llist_pos_init(&ret,pos.list_head_p->next,pos.llist);

	return ret;
}
list_pos_t llist_pos_prev(list_pos_t pos)
{
	list_pos_t ret;

	llist_pos_init(&ret,pos.list_head_p->prev,pos.llist);

	return ret;
}
int llist_pos_equal(list_pos_t pos1,list_pos_t pos2)
{
	return pos1.list_head_p == pos2.list_head_p;
}
llist_t *llist_create()
{
	llist_t *ret = NULL;
	ret = (llist_t *)mem_alloc_pck(NULL,sizeof(llist_t),0);
	if(ret == NULL){
		dbg_str(DBG_ERROR,"allock err");
	}
	return ret;
}
int llist_init(llist_t *llist,uint32_t data_size)
{
	struct list_head *p;
	dbg_str(DBG_DETAIL,"llist init");

	llist->data_size = data_size;
	llist->list_count = 0;
	//only assigned head,if without head,llist is hard to distinguish head or end;
	p = (struct list_head *)mem_alloc_pck(NULL,sizeof(struct list_head),0);
	if(p == NULL){
		dbg_str(DBG_ERROR,"allock err");
	}
	INIT_LIST_HEAD(p);
	llist_pos_init(&llist->begin,p,llist);
	llist_pos_init(&llist->end,p,llist);
	llist_pos_init(&llist->head,p,llist);

	pthread_rwlock_init(&llist->list_lock,NULL);

	return 0;
}
int llist_insert(llist_t *llist, list_pos_t pos, void *data)
{
	list_t *p = NULL;
	uint32_t data_size = llist->data_size; 

	dbg_str(DBG_IMPORTANT,"insert llist");
	p = (list_t *)mem_alloc_pck(NULL,sizeof(list_t) + data_size,0);
	memcpy(p->data,data,data_size);

	pthread_rwlock_wrlock(&llist->list_lock);
	list_add(&p->list_head, pos.list_head_p);
	if(llist_pos_equal(pos,llist->head)){
		llist_pos_init(&llist->begin,&p->list_head,llist);
	}
	if(llist_pos_equal(pos,llist->end)){
		llist_pos_init(&llist->end,&p->list_head,llist);
	}
	pthread_rwlock_unlock(&llist->list_lock);

	return 0;
}
int llist_delete(llist_t *llist, list_pos_t pos)
{
	list_t *p;

	p = container_of(pos.list_head_p,list_t,list_head);
	dbg_str(DBG_IMPORTANT,"delete llist");

	pthread_rwlock_wrlock(&llist->list_lock);
	if(llist_pos_equal(pos,llist->begin)){
		llist_pos_init(&llist->begin,pos.list_head_p->next,llist);
	}else if(llist_pos_equal(pos,llist->end)){
		llist_pos_init(&llist->end,pos.list_head_p->prev,llist);
	}
	list_del(pos.list_head_p);
	pthread_rwlock_unlock(&llist->list_lock);

	free(p);
	return 0;
}
int llist_push_front(llist_t *llist,void *data)
{
	return llist_insert(llist,llist->head,data);
}
int llist_push_back(llist_t *llist,void *data)
{
	return llist_insert(llist,llist->end,data);
}

int llist_pop_back(llist_t *llist)
{
	return llist_delete(llist, llist->end);
}
int llist_pop_front(llist_t *llist)
{
	return llist_delete(llist, llist->begin);
}

int llist_destroy(llist_t *llist)
{
	list_pos_t pos,next;

	dbg_str(DBG_IMPORTANT,"llist_destroy");

	for(	pos = llist_begin(llist), next = llist_pos_next(pos);
			!llist_pos_equal(pos,llist->head);
			pos = next, next = llist_pos_next(pos))
	{
		llist_delete(llist,pos);
	}
	if(llist_pos_equal(llist->head,llist->begin)){
		dbg_str(DBG_WARNNING,"llist_destroy,llist is NULL,free llist head");
		free(llist->head.list_head_p);
		pthread_rwlock_destroy(&llist->list_lock);
	}

	return 0;
}

void *llist_pos_get_pointer(list_pos_t pos)
{
	return (list_t *)(container_of(pos.list_head_p,list_t,list_head))->data;
}
void llist_for_each(llist_t *llist,void (*func)(list_t *list))
{
	list_t *ls;
	list_pos_t pos,next;


	for(	pos = llist_begin(llist), next = llist_pos_next(pos);
			!llist_pos_equal(pos,llist->head);
			pos = next, next = llist_pos_next(pos))
	{
		ls = container_of(pos.list_head_p,list_t,list_head);
		func(ls);
	}
}


/*
 *int main()
 *{
 *    llist_t *llist;
 *    struct test t1={1,2};
 *    struct test t2={2,2};
 *    struct test t3={3,2};
 *    struct test t4={4,2};
 *    int ret = 0;
 *
 *    llist = llist_create();
 *    llist_init(llist,sizeof(struct test));
 *    llist_push_front(llist,&t1);
 *    llist_push_front(llist,&t2);
 *    llist_push_front(llist,&t3);
 *    llist_push_front(llist,&t4);
 *
 *    llist_for_each(llist,print_list_data);
 *
 *    llist_destroy(llist);
 *    return ret;
 *}
 */
