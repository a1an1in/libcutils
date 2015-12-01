/*
 * =====================================================================================
 *
 *       Filename:  container_hash_map.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/10/2015 10:25:32 AM
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
#include "libcontainer/inc_files.h"
#include <libdata_structure/hash_list.h>


static inline int __container_hash_map_create(container_t *ct,uint8_t lock_type)
{
	dbg_str(DBG_CONTAINER_DETAIL,"__container_hash_map_create");
	ct->priv.hmap = hash_map_create(ct->allocator,lock_type);
	if(ct->priv.hmap == NULL){
		return 0;
	}else{
		return 1;
	}
}
static inline int __container_hash_map_init(container_t *ct,uint32_t data_size)
{
	hash_map_t *hmap =  ct->priv.hmap;

	dbg_str(DBG_CONTAINER_DETAIL,"__container_hash_map_init");

	return  hash_map_init(hmap,//hash_map_t *hmap,
			hmap->key_size,//uint32_t key_size,
			data_size,//uint32_t data_size,
			hmap->bucket_size,//uint32_t bucket_size,
			hmap->hash_func,//hash_func_fpt hash_func,
			hmap->key_cmp_func);//key_cmp_fpt key_cmp_func);
}

static inline int __container_hash_map_insert(container_t *ct, void *data)
{
	return hash_map_insert(ct->priv.hmap,data);
}
static inline iterator_t * 
__container_hash_map_search(container_t *ct, void *key,iterator_t *it)
{
	it->container_p = ct;
	hash_map_search(ct->priv.hmap, key,&it->pos.hash_pos);
	return it;
}
static inline int __container_hash_map_delete(container_t *ct, iterator_t *it)
{
	return hash_map_delete(ct->priv.hmap, &it->pos.hash_pos);
}
static inline int __container_hash_map_destroy(container_t *ct)
{
	return  hash_map_destroy(ct->priv.hmap);
}

static inline iterator_t * 
__container_hash_map_iterator_init(iterator_t *it,
		struct hlist_node *hlist_node_p,
		uint32_t bucket_pos,
		struct hlist_head *hlist,
		container_t *ct)
{
	it->container_p = ct;
	hash_map_pos_init(&it->pos.hash_pos,
			hlist_node_p,
			bucket_pos,
			hlist,
			ct->priv.hmap);
}
static inline iterator_t *
__container_hash_map_begin(container_t *ct,iterator_t *begin)
{
	begin->container_p = ct;
	hash_map_begin(ct->priv.hmap,&begin->pos.hash_pos);
	return begin;
}

static inline iterator_t *
__container_hash_map_end(container_t *ct,iterator_t *end)
{
	end->container_p = ct;
	hash_map_end(ct->priv.hmap,&end->pos.hash_pos);
	return end;
}
static inline iterator_t *
__container_hash_map_iterator_next(iterator_t *it,iterator_t * next)
{
	next->container_p = it->container_p;
	hash_map_pos_next(&it->pos.hash_pos,&next->pos.hash_pos);
	return next;
}
static inline iterator_t*
__container_hash_map_iterator_prev(iterator_t *it,iterator_t *prev)
{
	return NULL;
}
static inline int __container_hash_map_iterator_equal(iterator_t *it1,iterator_t *it2)
{
	return hash_map_pos_equal(&it1->pos.hash_pos,&it2->pos.hash_pos);
}
static inline void *__container_hash_map_iterator_get_pointer(iterator_t *it)
{
	return hash_map_pos_get_pointer(&it->pos.hash_pos);
}

int container_hash_map_register(){
	container_module_t cm = {
		.name = "hash_map",
		.container_type = CONTAINER_TYPE_HASH_MAP,
		.c_ops = {
			.create     = __container_hash_map_create,
			.init       = __container_hash_map_init,
			.begin      = __container_hash_map_begin,
			.end        = __container_hash_map_end,
			.map_search = __container_hash_map_search,
			.del        = __container_hash_map_delete,
			.map_insert = __container_hash_map_insert,
			.destroy    = __container_hash_map_destroy,
		},
		.it_ops = {
			.next  = __container_hash_map_iterator_next,
			.prev  = __container_hash_map_iterator_prev,
			.equal = __container_hash_map_iterator_equal,
			.iterator_get_pointer = __container_hash_map_iterator_get_pointer
		}
	};
	printf("hmap regist module addr:%p",&container_modules[CONTAINER_TYPE_HASH_MAP].c_ops);
	memcpy(&container_modules[CONTAINER_TYPE_HASH_MAP],&cm,sizeof(container_module_t));
	return 0;
}

