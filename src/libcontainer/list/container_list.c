/*
 * =====================================================================================
 *
 *       Filename:  container_list.c
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
#include <pthread.h>
#include "libcontainer/container_list.h"
#include "libcontainer/inc_files.h"

int list_iterator_init(iterator_t *it,struct list_head *lh,container_t *ct);
int list_iterator_equal(iterator_t it1,iterator_t it2);
// free list not considered
int list_init(container_t *ct)
{
	struct container_list *p = NULL;
	uint32_t data_size = ct->priv.list_priv.data_size; 

	dbg_str(DBG_CONTAINER_DETAIL,"list init");

	ct->container_type = CONTAINER_TYPE_LIST;
	strcpy(ct->name,"list container");
	p = (struct container_list *)allocator_mem_alloc(ct->allocator,sizeof(struct container_list) + data_size);
	INIT_LIST_HEAD(&p->list_head);
	list_iterator_init(&ct->begin,&p->list_head,ct);
	list_iterator_init(&ct->end,&p->list_head,ct);
	list_iterator_init(&ct->cur,&p->list_head,ct);
	return 0;
}
int list_push_front(container_t *ct,void *data)
{
	struct container_list *p = NULL;
	uint32_t data_size = ct->priv.list_priv.data_size; 

	p = (struct container_list *)allocator_mem_alloc(ct->allocator,sizeof(struct container_list) + data_size);
	memcpy(p->data,data,data_size);

	pthread_rwlock_wrlock(&ct->head_lock);
	list_add(&p->list_head, ct->end.pos.list_head_p);//end can be regrad as head list
	list_iterator_init(&ct->begin,&p->list_head,ct);
	pthread_rwlock_unlock(&ct->head_lock);

	return 0;
}
int list_push_back(container_t *ct,void *data)
{
	struct container_list *p = NULL;
	uint32_t data_size = ct->priv.list_priv.data_size; 

	p = (struct container_list *)allocator_mem_alloc(ct->allocator,sizeof(struct container_list) + data_size);
	memcpy(p->data,data,data_size);

	pthread_rwlock_wrlock(&ct->head_lock);
	list_add_tail(&p->list_head, ct->end.pos.list_head_p);
	list_iterator_init(&ct->begin,ct->end.pos.list_head_p->next,ct);
	pthread_rwlock_unlock(&ct->head_lock);

	return 0;
}

int list_pop_back(container_t *ct)
{
	struct container_list *p;
	struct list_head *head = ct->end.pos.list_head_p;
	
	p = container_of(head->prev,struct container_list,list_head);
	dbg_str(DBG_CONTAINER_DETAIL,"pop back");

	pthread_rwlock_wrlock(&ct->head_lock);
	list_del(head->prev);
	pthread_rwlock_unlock(&ct->head_lock);

	allocator_mem_free(ct->allocator,p);

	return 0;
}
int list_pop_front(container_t *ct)
{
	struct container_list *p;
	struct list_head *head = ct->end.pos.list_head_p;
	
	p = container_of(head->next,struct container_list,list_head);
	dbg_str(DBG_CONTAINER_DETAIL,"pop front");

	pthread_rwlock_wrlock(&ct->head_lock);
	list_del(head->next);
	list_iterator_init(&ct->begin,head->next,ct);
	pthread_rwlock_unlock(&ct->head_lock);

	allocator_mem_free(ct->allocator,p);

	return 0;
}
int list_insert(container_t *ct, iterator_t it, void *data)
{
	struct container_list *p = NULL;
	uint32_t data_size = ct->priv.list_priv.data_size; 

	dbg_str(DBG_CONTAINER_IMPORTANT,"insert list");
	p = (struct container_list *)allocator_mem_alloc(ct->allocator,sizeof(struct container_list) + data_size);
	memcpy(p->data,data,data_size);

	pthread_rwlock_wrlock(&ct->head_lock);
	list_add(&p->list_head, it.pos.list_head_p);
	list_iterator_init(&ct->begin,ct->end.pos.list_head_p->next,ct);
	pthread_rwlock_unlock(&ct->head_lock);

	return 0;
}
int list_delete(container_t *ct, iterator_t it)
{
	struct container_list *p;

	p = container_of(it.pos.list_head_p,struct container_list,list_head);
	dbg_str(DBG_CONTAINER_IMPORTANT,"delete list");

	pthread_rwlock_wrlock(&ct->head_lock);
	if(list_iterator_equal(it,ct->begin)){
		list_iterator_init(&ct->begin,it.pos.list_head_p->next,ct);
	}
	list_del(it.pos.list_head_p);
	pthread_rwlock_unlock(&ct->head_lock);

	allocator_mem_free(ct->allocator,p);
	return 0;
}
int list_destroy(container_t *ct)
{
	iterator_t it,next;

	dbg_str(DBG_CONTAINER_IMPORTANT,"list_destroy");
	for(	it = container_begin(ct), next = iterator_next(it);
			!iterator_equal(it,container_end(ct));
			it = next, next = iterator_next(it))
	{
		list_delete(ct,it);
	}
	if(list_iterator_equal(ct->end,ct->begin)){
		dbg_str(DBG_CONTAINER_WARNNING,"list_destroy,list is NULL,free list head");
		allocator_mem_free(ct->allocator,ct->end.pos.list_head_p);
	}
}

iterator_t list_begin(container_t *ct)
{
	return ct->begin;
}

iterator_t list_end(container_t *ct)
{
	return ct->end;
}


int list_iterator_init(iterator_t *it,struct list_head *lh,container_t *ct)
{

	it->container_p = ct;
	it->pos.list_head_p = lh;

	return 0;
}
iterator_t list_iterator_next(iterator_t it)
{
	iterator_t ret;

	list_iterator_init(&ret,it.pos.list_head_p->next,it.container_p);

	return ret;
}
iterator_t list_iterator_prev(iterator_t it)
{
	iterator_t ret;

	list_iterator_init(&ret,it.pos.list_head_p->prev,it.container_p);

	return ret;
}
int list_iterator_equal(iterator_t it1,iterator_t it2)
{
	return it1.pos.list_head_p == it2.pos.list_head_p;
}
void *list_iterator_get_pointer(iterator_t it)
{
	return (struct container_list *)(container_of(it.pos.list_head_p,struct container_list,list_head))->data;
}

int  container_list_register(){
	container_module_t cm = {
		.name = "container_list",
		.container_type = CONTAINER_TYPE_LIST,
		.c_ops = {
			.init       = list_init,
			.push_front = list_push_front,
			.push_back  = list_push_back,
			.pop_front  = list_pop_front,
			.pop_back   = list_pop_back,
			.begin      = list_begin,
			.end        = list_end,
			.del        = list_delete,
			.insert     = list_insert,
			.destroy 	= list_destroy,
		},
		.it_ops = {
			list_iterator_next,
			list_iterator_prev,
			list_iterator_equal,
			list_iterator_get_pointer
		}
	};
	memcpy(&container_modules[CONTAINER_TYPE_LIST],&cm,sizeof(container_module_t));
	return 0;
}

