/*
 * =====================================================================================
 *
 *       Filename:  container_rbtree_map.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/01/2015 08:14:09 PM
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "libcontainer/container.h"
#include <libdata_structure/rbtree_map.h>


static inline int __container_rbtree_map_create(container_t *ct,uint8_t lock_type)
{
	dbg_str(DBG_CONTAINER_DETAIL,"__container_rbtree_map_create");
	ct->priv.tmap = rbtree_map_create(ct->allocator,lock_type);
	if(ct->priv.tmap == NULL){
		return 0;
	}else{
		return 1;
	}
}
/*
 *int rbtree_map_init(rbtree_map_t *map,
 *        uint32_t key_size,
 *        uint32_t data_size,
 *        key_cmp_fpt key_cmp_func)
 */
static inline int __container_rbtree_map_init(container_t *ct,uint32_t data_size)
{
	rbtree_map_t *tmap =  ct->priv.tmap;

	dbg_str(DBG_CONTAINER_DETAIL,"__container_rbtree_map_init");

	return  rbtree_map_init(tmap,//rbtree_map_t *tmap,
			tmap->key_size,//uint32_t key_size,
			data_size,//uint32_t data_size,
			tmap->key_cmp_func);//key_cmp_fpt key_cmp_func);
}

static inline int __container_rbtree_map_insert(container_t *ct, void *data)
{
	return rbtree_map_insert(ct->priv.tmap,data);
}
static inline iterator_t * 
__container_rbtree_map_search(container_t *ct, void *key,iterator_t *it)
{
	it->container_p = ct;
	rbtree_map_search(ct->priv.tmap, key,&it->pos.rbtree_pos);
	return it;
}
static inline int __container_rbtree_map_delete(container_t *ct, iterator_t *it)
{
	return rbtree_map_delete(ct->priv.tmap, &it->pos.rbtree_pos);
}
static inline int __container_rbtree_map_destroy(container_t *ct)
{
	return  rbtree_map_destroy(ct->priv.tmap);
}

static inline iterator_t *
__container_rbtree_map_begin(container_t *ct,iterator_t *begin)
{
	begin->container_p = ct;
	rbtree_map_begin(ct->priv.tmap,&begin->pos.rbtree_pos);
	return begin;
}

static inline iterator_t *
__container_rbtree_map_end(container_t *ct,iterator_t *end)
{
	end->container_p = ct;
	rbtree_map_end(ct->priv.tmap,&end->pos.rbtree_pos);
	return end;
}
static inline iterator_t *
__container_rbtree_map_iterator_next(iterator_t *it,iterator_t * next)
{
	next->container_p = it->container_p;
	rbtree_map_pos_next(&it->pos.rbtree_pos,&next->pos.rbtree_pos);
	return next;
}
static inline iterator_t*
__container_rbtree_map_iterator_prev(iterator_t *it,iterator_t *prev)
{
	return NULL;
}
static inline int __container_rbtree_map_iterator_equal(iterator_t *it1,iterator_t *it2)
{
	return rbtree_map_pos_equal(&it1->pos.rbtree_pos,&it2->pos.rbtree_pos);
}
static inline void *__container_rbtree_map_iterator_get_pointer(iterator_t *it)
{
	return rbtree_map_pos_get_pointer(&it->pos.rbtree_pos);
}

int container_rbtree_map_register(){
	container_module_t cm = {
		.name = "rbtree_map",
		.container_type = CONTAINER_TYPE_RBTREE_MAP,
		.c_ops = {
			.create     = __container_rbtree_map_create,
			.init       = __container_rbtree_map_init,
			.begin      = __container_rbtree_map_begin,
			.end        = __container_rbtree_map_end,
			.map_search = __container_rbtree_map_search,
			.del        = __container_rbtree_map_delete,
			.map_insert = __container_rbtree_map_insert,
			.destroy    = __container_rbtree_map_destroy,
		},
		.it_ops = {
			.next  = __container_rbtree_map_iterator_next,
			.prev  = __container_rbtree_map_iterator_prev,
			.equal = __container_rbtree_map_iterator_equal,
			.iterator_get_pointer = __container_rbtree_map_iterator_get_pointer
		}
	};
	printf("tmap regist module addr:%p",&container_modules[CONTAINER_TYPE_RBTREE_MAP].c_ops);
	memcpy(&container_modules[CONTAINER_TYPE_RBTREE_MAP],&cm,sizeof(container_module_t));
	return 0;
}

