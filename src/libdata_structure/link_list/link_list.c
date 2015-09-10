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
#include "libdbg/debug.h"
#include "libdata_structure/link_list.h"


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
llist_t *llist_create(allocator_t *allocator,uint8_t lock_type)
{
	llist_t *ret = NULL;
	ret = (llist_t *)allocator_mem_alloc(allocator,sizeof(llist_t));
	if(ret == NULL){
		dbg_str(DBG_ERROR,"allock err");
	}
	ret->allocator = allocator;
	ret->lock_type = lock_type;
	return ret;
}
int llist_init(llist_t *llist,uint32_t data_size)
{
	struct list_head *p;
	dbg_str(DBG_DETAIL,"llist init");

	llist->data_size = data_size;
	llist->list_count = 0;
	//only assigned head,if without head,llist is hard to distinguish head or end;
	p = (struct list_head *)allocator_mem_alloc(llist->allocator, sizeof(struct list_head));
	if(p == NULL){
		dbg_str(DBG_ERROR,"allock err");
	}
	INIT_LIST_HEAD(p);
	llist_pos_init(&llist->begin,p,llist);
	llist_pos_init(&llist->end,p,llist);
	llist_pos_init(&llist->head,p,llist);

	/*
	 *sync_lock_init(&llist->list_lock,0);
	 */
	sync_lock_init(&llist->list_lock,llist->lock_type);
	/*
	 *pthread_rwlock_init(&llist->list_lock,NULL);
	 */

	return 0;
}
int llist_insert(llist_t *llist, list_pos_t pos, void *data)
{
	list_t *p = NULL;
	uint32_t data_size = llist->data_size; 

	p = (list_t *)allocator_mem_alloc(llist->allocator,sizeof(list_t) + data_size);
	memcpy(p->data,data,data_size);

	sync_lock(&llist->list_lock,NULL);
	/*
	 *pthread_rwlock_wrlock(&llist->list_lock);
	 */
	list_add(&p->list_head, pos.list_head_p);
	if(llist_pos_equal(pos,llist->head)){
		llist_pos_init(&llist->begin,&p->list_head,llist);
	}
	if(llist_pos_equal(pos,llist->end)){
		llist_pos_init(&llist->end,&p->list_head,llist);
	}
	llist->list_count++;
	dbg_str(DBG_IMPORTANT,"insert llist,listcount=%d",llist->list_count);

	sync_unlock(&llist->list_lock);
	/*
	 *pthread_rwlock_unlock(&llist->list_lock);
	 */

	return 0;
}
int llist_delete(llist_t *llist, list_pos_t pos)
{
	list_t *p;

	p = container_of(pos.list_head_p,list_t,list_head);

	sync_lock(&llist->list_lock,NULL);
	/*
	 *pthread_rwlock_wrlock(&llist->list_lock);
	 */
	if(llist_pos_equal(pos,llist->begin)){
		llist_pos_init(&llist->begin,pos.list_head_p->next,llist);
	}else if(llist_pos_equal(pos,llist->end)){
		llist_pos_init(&llist->end,pos.list_head_p->prev,llist);
	}
	list_del(pos.list_head_p);
	llist->list_count--;
	dbg_str(DBG_IMPORTANT,"delete llist,listcount=%d",llist->list_count);

	sync_unlock(&llist->list_lock);
	/*
	 *pthread_rwlock_unlock(&llist->list_lock);
	 */

	allocator_mem_free(llist->allocator,p);
	return 0;
}
list_t *llist_detach(llist_t *llist, list_pos_t pos)
{
	list_t *p;

	p = container_of(pos.list_head_p,list_t,list_head);

	sync_lock(&llist->list_lock,NULL);
	/*
	 *pthread_rwlock_wrlock(&llist->list_lock);
	 */
	if(llist_pos_equal(pos,llist->begin)){
		llist_pos_init(&llist->begin,pos.list_head_p->next,llist);
	}else if(llist_pos_equal(pos,llist->end)){
		llist_pos_init(&llist->end,pos.list_head_p->prev,llist);
	}
	list_del(pos.list_head_p);
	llist->list_count--;
	dbg_str(DBG_IMPORTANT,"detach llist,listcount=%d",llist->list_count);
	/*
	 *pthread_rwlock_unlock(&llist->list_lock);
	 */
	sync_unlock(&llist->list_lock);

	return p;
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

list_t *llist_detach_back(llist_t *llist)
{
	return llist_detach(llist, llist->end);
}
list_t *llist_detach_front(llist_t *llist)
{
	return llist_detach(llist, llist->begin);
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
		allocator_mem_free(llist->allocator,llist->head.list_head_p);
		/*
		 *pthread_rwlock_destroy(&llist->list_lock);
		 */
		sync_lock_destroy(&llist->list_lock);
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
