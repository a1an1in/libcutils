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
#include <libdata_structure/link_list.h>
#include "libcontainer/inc_files.h"

// free list not considered
static inline int __container_list_create(container_t *ct,uint8_t lock_type)
{
	ct->priv.llist = llist_create(ct->allocator,lock_type);
	if(ct->priv.llist != NULL){
		return 1;
	} else{
		dbg_str(DBG_ERROR,"container_list create err");
		return 0;
	}
}
static inline int __container_list_init(container_t *ct,uint32_t data_size)
{
	return llist_init(ct->priv.llist,data_size);
}
static inline int __container_list_push_front(container_t *ct,void *data)
{
	return llist_push_front(ct->priv.llist,data);
}
static inline int __container_list_push_back(container_t *ct,void *data)
{
	return llist_push_back(ct->priv.llist,data);
}
static inline int __container_list_pop_back(container_t *ct)
{
	return llist_pop_back(ct->priv.llist);
}
static inline int __container_list_pop_front(container_t *ct)
{
	return llist_pop_front(ct->priv.llist);
}
static inline int __container_list_insert(container_t *ct, iterator_t *it, void *data)
{
	return llist_insert(ct->priv.llist,&it->pos.list_pos,data);
}
static inline int __container_list_delete(container_t *ct, iterator_t *it)
{
	return llist_delete(ct->priv.llist,&it->pos.list_pos);
}
static inline int __container_list_destroy(container_t *ct)
{
	return llist_destroy(ct->priv.llist);
}

static inline iterator_t *
__container_list_begin(container_t *ct,iterator_t  *begin)
{
	begin->container_p = ct;
	llist_begin(ct->priv.llist,&begin->pos.list_pos);
	return begin;
}

static inline iterator_t *
__container_list_end(container_t *ct,iterator_t *end)
{
	end->container_p = ct;
	llist_end(ct->priv.llist,&end->pos.list_pos);
	return end;
}

static inline iterator_t *
__container_list_iterator_init(iterator_t *it,struct list_head *lh,container_t *ct)
{
	it->container_p = ct;
	llist_pos_init(&it->pos.list_pos,lh,ct->priv.llist);
	return it;
}
static inline iterator_t *
__container_list_iterator_next(iterator_t *it,iterator_t *next)
{
	next->container_p = it->container_p;
	llist_pos_next(&it->pos.list_pos,&next->pos.list_pos);
	return next;
}
static inline iterator_t *
__container_list_iterator_prev(iterator_t *it,iterator_t *prev)
{
	prev->container_p = it->container_p;
	llist_pos_prev(&it->pos.list_pos,&prev->pos.list_pos);
	return prev;
}
static inline int __container_list_iterator_equal(iterator_t *it1,iterator_t *it2)
{
	return llist_pos_equal(&it1->pos.list_pos,&it2->pos.list_pos);
}
static inline void *__container_list_iterator_get_pointer(iterator_t *it)
{
	return llist_pos_get_pointer(&it->pos.list_pos);
}

int  container_list_register(){
	container_module_t cm = {
		.name = "container_list",
		.container_type = CONTAINER_TYPE_LIST,
		.c_ops = {
			.create     = __container_list_create,
			.init       = __container_list_init,
			.push_front = __container_list_push_front,
			.push_back  = __container_list_push_back,
			.pop_front  = __container_list_pop_front,
			.pop_back   = __container_list_pop_back,
			.begin      = __container_list_begin,
			.end        = __container_list_end,
			.del        = __container_list_delete,
			.insert     = __container_list_insert,
			.destroy    = __container_list_destroy,
		},
		.it_ops = {
			__container_list_iterator_next,
			__container_list_iterator_prev,
			__container_list_iterator_equal,
			__container_list_iterator_get_pointer
		}
	};
	memcpy(&container_modules[CONTAINER_TYPE_LIST],&cm,sizeof(container_module_t));
	return 0;
}

