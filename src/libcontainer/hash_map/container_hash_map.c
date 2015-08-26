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
#include "libcontainer/container_hash_map.h"
#include "libcontainer/inc_files.h"


iterator_t __hash_map_iterator_init(iterator_t *it, struct hlist_node *hlist_node_p, uint32_t bucket_pos, struct hlist_head *hlist, container_t *ct);
iterator_t __hash_map_iterator_next(iterator_t it);
int __hash_map_iterator_equal(iterator_t it1,iterator_t it2);

static int default_key_cmp_func(void *key1,void *key2,uint32_t size)
{
	return memcmp(key1,key2,size);
}
static uint32_t default_hash_func(void *key,uint32_t key_size,uint32_t bucket_size)
{
	uint32_t sum = 0;
	uint32_t i = 0;
	for(i = 0; i < key_size; i++){
		sum += *(uint8_t *)(key + i);
	}
	return sum % bucket_size;
}
int __hash_map_init(container_t *ct)
{

	uint32_t bucket_size     = ct->priv.hash_map_priv.bucket_size;;
	hash_func_fpt hash_func  = ct->priv.hash_map_priv.hash_func;
	key_cmp_fpt key_cmp_func = ct->priv.hash_map_priv.key_cmp_fpt;
	struct hlist_head *hlist;

	dbg_str(DBG_CONTAINER_DETAIL,"hash_map init");
	strcpy(ct->name,"hash_map container");

	if(key_cmp_func == NULL){
		ct->priv.hash_map_priv.key_cmp_fpt = default_key_cmp_func;
	}
	if(hash_func == NULL){
		ct->priv.hash_map_priv.hash_func = default_hash_func;
	}
	hlist = (struct hlist_head *)allocator_mem_alloc(ct->allocator,
			sizeof(struct hlist_head)*bucket_size);
	memset(hlist,0,sizeof(struct hlist_head)*bucket_size);

	__hash_map_iterator_init(&ct->begin,
			NULL,//struct hlist_node *hlist_node_p,
			0,//uint32_t bucket_pos,
			hlist,//struct hlist_head *hlist
			ct);
	__hash_map_iterator_init(&ct->end,
			NULL,
			bucket_size - 1,
			hlist,
			ct);

	return 0;
}

iterator_t __hash_map_begin(container_t *ct)
{
	return ct->begin;
}

iterator_t __hash_map_end(container_t *ct)
{
	return ct->end;
}
int __hash_map_insert(container_t *ct, void *data)
{
	struct hash_map_node *mnode;
	uint32_t bucket_pos;
	uint32_t data_size       = ct->priv.hash_map_priv.data_size;
	uint32_t key_size        = ct->priv.hash_map_priv.key_size;
	struct hlist_head *hlist = ct->begin.pos.hash_map_pos.hlist;
	hash_func_fpt hash_func  = ct->priv.hash_map_priv.hash_func;
	uint32_t bucket_size     = ct->priv.hash_map_priv.bucket_size;;
	hash_map_pos_t *__hash_map_begin_pos = &ct->begin.pos.hash_map_pos;

	dbg_str(DBG_CONTAINER_DETAIL,"__hash_map_insert");

	mnode = (struct hash_map_node *)allocator_mem_alloc(ct->allocator,
			sizeof(struct hash_map_node) + data_size);
	if(mnode == NULL){
		dbg_str(DBG_CONTAINER_ERROR,"__hash_map_insert,malloc err");
		return -1;
	}

	memcpy(mnode->key,data,data_size);
	mnode->value_pos = key_size;

	INIT_HLIST_NODE(&mnode->hlist_node);

	bucket_pos = hash_func(mnode->key,key_size,bucket_size); 
	assert(bucket_pos <= bucket_size);

	/*
	 *pthread_rwlock_wrlock(&ct->head_lock);
	 */
	sync_lock(&ct->head_lock,NULL);

	hlist_add_head(&mnode->hlist_node, &hlist[bucket_pos]);
	if(__hash_map_begin_pos->hlist_node_p == NULL || bucket_pos <= __hash_map_begin_pos->bucket_pos){
		__hash_map_iterator_init(&ct->begin, hlist[bucket_pos].first, bucket_pos, hlist, ct);
		dbg_str(DBG_CONTAINER_WARNNING,"change begin pos");
	}
	dbg_str(DBG_CONTAINER_IMPORTANT,"bucket_pos =%d,first =%p,next=%p,begin.hash_map_pos=%p",
			bucket_pos,
			hlist[bucket_pos].first,
			hlist[bucket_pos].first->next,
			ct->begin.pos.hash_map_pos.hlist_node_p);
	/*
	 *pthread_rwlock_unlock(&ct->head_lock);
	 */
	sync_unlock(&ct->head_lock);

	return 0;
}
iterator_t __hash_map_search(container_t *ct, void *key)
{
	iterator_t ret;
	struct hash_map_node *mnode;
	uint32_t key_size        = ct->priv.hash_map_priv.key_size;
	struct hlist_head *hlist = ct->begin.pos.hash_map_pos.hlist;
	hash_func_fpt hash_func  = ct->priv.hash_map_priv.hash_func;
	key_cmp_fpt key_cmp_func = ct->priv.hash_map_priv.key_cmp_fpt;
	uint32_t bucket_size     = ct->priv.hash_map_priv.bucket_size;;
	struct hlist_node *pos,*next;
	uint32_t bucket_pos;


	dbg_str(DBG_CONTAINER_DETAIL,"__hash_map_search");

	bucket_pos = hash_func(key,key_size,bucket_size); 
	assert(bucket_pos <= bucket_size);

	dbg_str(DBG_CONTAINER_DETAIL,"__hash_map_search,bucket_pos=%d",bucket_pos);
	/*
	 *pthread_rwlock_rdlock(&ct->head_lock);
	 */
	sync_lock(&ct->head_lock,NULL);
	hlist_for_each_safe(pos, next, &hlist[bucket_pos]){
		mnode = container_of(pos,struct hash_map_node,hlist_node);
		if(!key_cmp_func(mnode->key,key,key_size)){
			dbg_str(DBG_CONTAINER_IMPORTANT,"found key");
			sync_unlock(&ct->head_lock);
			/*
			 *pthread_rwlock_unlock(&ct->head_lock);
			 */
			return __hash_map_iterator_init(&ret, pos, bucket_pos, hlist, ct);
		}
	}
	/*
	 *pthread_rwlock_unlock(&ct->head_lock);
	 */
	sync_unlock(&ct->head_lock);
	if(mnode == NULL){
		return __hash_map_iterator_init(&ret, NULL, bucket_pos, hlist, ct);
	}

	return ret;
}
int __hash_map_delete(container_t *ct, iterator_t it)
{
	struct hash_map_node *mnode;
	hash_map_pos_t *it_hm_pos = &it.pos.hash_map_pos;
	struct hlist_head *hlist = it_hm_pos->hlist;
	iterator_t next;
	hash_map_pos_t *next_hm_pos = &next.pos.hash_map_pos;

	dbg_str(DBG_CONTAINER_IMPORTANT,"del hash_map ,bucket_pos:%d,cur node:%p,begin node:%p",
			it_hm_pos->bucket_pos,
			it_hm_pos->hlist_node_p,
			ct->begin.pos.hash_map_pos.hlist_node_p);
	/*
	 *pthread_rwlock_wrlock(&ct->head_lock);
	 */
	sync_lock(&ct->head_lock,0);
	if(__hash_map_iterator_equal(it,ct->begin)){
		dbg_str(DBG_CONTAINER_WARNNING,"del iter equal begain");
		next = __hash_map_iterator_next(it);
		next_hm_pos = &next.pos.hash_map_pos;
		dbg_str(DBG_CONTAINER_DETAIL,"del next node:%p",next_hm_pos->hlist_node_p);
		__hash_map_iterator_init(&ct->begin,
				next_hm_pos->hlist_node_p, 
				next_hm_pos->bucket_pos, hlist, ct);
	}
	hlist_del(it_hm_pos->hlist_node_p);
	/*
	 *pthread_rwlock_unlock(&ct->head_lock);
	 */
	sync_unlock(&ct->head_lock);

	mnode = container_of(it.pos.hash_map_pos.hlist_node_p,struct hash_map_node,hlist_node);
	if (mnode != NULL) {
		allocator_mem_free(ct->allocator,mnode);
		mnode = NULL;
	}
	return 0;
}
int __hash_map_destroy(container_t *ct)
{
	 dbg_str(DBG_CONTAINER_DETAIL,"__hash_map_destroy");
	 iterator_t it;
	 for(	 it = __hash_map_begin(ct);
			 !__hash_map_iterator_equal(it,__hash_map_end(ct));
			 it = __hash_map_begin(ct))
	 {
		 dbg_str(DBG_CONTAINER_DETAIL,"destroy node:%p",it.pos.hash_map_pos.hlist_node_p);
		 __hash_map_delete(ct,it);
	 }
	 if(__hash_map_iterator_equal(ct->end,it)){
		 dbg_str(DBG_CONTAINER_WARNNING,"__hash_map_destroy,hash_map is NULL");
		 allocator_mem_free(ct->allocator,ct->end.pos.hash_map_pos.hlist);
	 }
}

iterator_t __hash_map_iterator_init(iterator_t *it,
		struct hlist_node *hlist_node_p,
		uint32_t bucket_pos,
		struct hlist_head *hlist,
		container_t *ct)
{
	hash_map_pos_t *hm_pos = &it->pos.hash_map_pos;
	it->container_p = ct;
	hm_pos->hlist_node_p = hlist_node_p;
	hm_pos->bucket_pos = bucket_pos;
	hm_pos->hlist = hlist;
	return *it;
}
iterator_t __hash_map_iterator_next(iterator_t it)
{
	hash_map_pos_t *hm_pos    = &it.pos.hash_map_pos;
	struct hlist_node *hlist_node_p;
	uint32_t bucket_pos      = hm_pos->bucket_pos;
	struct hlist_head *hlist = hm_pos->hlist;
	container_t *ct          = it.container_p;
	uint32_t bucket_size     = ct->priv.hash_map_priv.bucket_size;

	if(hm_pos->hlist_node_p->next){
		dbg_str(DBG_CONTAINER_DETAIL,"find next iter");
		hlist_node_p = hm_pos->hlist_node_p->next;
		__hash_map_iterator_init(&it, hlist_node_p, bucket_pos, hlist, ct);
		dbg_str(DBG_CONTAINER_DETAIL,"bucket_pos=%d,node:%p,next:%p",
				bucket_pos,hlist_node_p,hlist_node_p->next);
		return it;
	} else if(bucket_pos < bucket_size - 1 ){
		for(++bucket_pos; bucket_pos < bucket_size;bucket_pos++){
			if(hlist[bucket_pos].first){
				dbg_str(DBG_CONTAINER_DETAIL,"find next iter");
				hlist_node_p = hlist[bucket_pos].first;
				__hash_map_iterator_init(&it, hlist_node_p, bucket_pos, hlist, ct);
				dbg_str(DBG_CONTAINER_DETAIL,"bucket_pos=%d,node:%p,next:%p",
						bucket_pos,hlist_node_p,hlist_node_p->next);
				return it;
			}
		}
		__hash_map_iterator_init(&it, NULL, bucket_size -1, hlist, ct);
	} else if(bucket_pos == bucket_size -1 ){
		dbg_str(DBG_CONTAINER_IMPORTANT,"container is null");
		__hash_map_iterator_init(&it, NULL, bucket_pos, hlist, ct);
	}else{
		dbg_str(DBG_CONTAINER_ERROR,"__hash_map_iterator_next err");
	}

	return it;
}
iterator_t __hash_map_iterator_prev(iterator_t it)
{
	return it;
}
int __hash_map_iterator_equal(iterator_t it1,iterator_t it2)
{
	hash_map_pos_t *hm_pos1 = &it1.pos.hash_map_pos;
	hash_map_pos_t *hm_pos2 = &it2.pos.hash_map_pos;

	return (hm_pos1->hlist_node_p == hm_pos2->hlist_node_p) &&
		   (hm_pos1->bucket_pos   == hm_pos2->bucket_pos);
}
void *__hash_map_iterator_get_pointer(iterator_t it)
{
	struct hash_map_node *mnode;

	mnode = container_of(it.pos.hash_map_pos.hlist_node_p,
			struct hash_map_node,
			hlist_node);

	return &mnode->key[mnode->value_pos];
}

int container_hash_map_register(){
	container_module_t cm = {
		.name = "hash_map",
		.container_type = CONTAINER_TYPE_HASH_MAP,
		.c_ops = {
			.init       = __hash_map_init,
			.begin      = __hash_map_begin,
			.end        = __hash_map_end,
			.map_search = __hash_map_search,
			.del        = __hash_map_delete,
			.map_insert = __hash_map_insert,
			.destroy    = __hash_map_destroy,
		},
		.it_ops = {
			.next  = __hash_map_iterator_next,
			.prev  = __hash_map_iterator_prev,
			.equal = __hash_map_iterator_equal,
			.iterator_get_pointer = __hash_map_iterator_get_pointer
		}
	};
	memcpy(&container_modules[CONTAINER_TYPE_HASH_MAP],&cm,sizeof(container_module_t));
	return 0;
}

