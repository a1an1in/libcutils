/*
 * =====================================================================================
 *
 *       Filename:  hash_list.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/12/2015 03:25:59 PM
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
#include <assert.h>
#include "libdbg/debug.h"
#include "libdatastructure/hash_list.h"
#include "libdatastructure/memory_management.h"

int hash_map_init(hash_map_t **hmap,
		uint32_t key_size,
		uint32_t data_size,
		uint32_t bucket_size,
		hash_func_fpt hash_func,
		key_cmp_fpt key_cmp_func)
{
	hash_map_t *map;
	map = mem_alloc_pck(NULL,sizeof(hash_map_t),0);
	if(map == NULL){
		dbg_str(DBG_ERROR,"malloc err");
	}
	map->data_size = data_size;
	map->key_size = key_size;
	map->bucket_size = bucket_size;
	map->hash_func = hash_func;
	map->key_cmp_func = key_cmp_func;

	map->hlist = malloc(sizeof(struct hlist_head)*bucket_size);
	memset(map->hlist,0,sizeof(struct hlist_head)*bucket_size);

	hash_map_pos_init(&map->begin,NULL,0,map->hlist,map);
	hash_map_pos_init(&map->end,NULL,bucket_size - 1,map->hlist,map);

	pthread_rwlock_init(&map->map_lock,NULL);
	*hmap = map;
	
	return 0;
}
hash_map_pos_t hash_map_pos_init(hash_map_pos_t *pos,
		struct hlist_node *hlist_node_p,
		uint32_t bucket_pos,
		struct hlist_head *hlist_head_p,
		struct hash_map_s *hmap)
{
	pos->hlist_node_p = hlist_node_p;
	pos->bucket_pos   = bucket_pos;
	pos->hlist        = hlist_head_p;
	pos->hmap         = hmap;

	return *pos;
}
hash_map_pos_t hash_map_begin(hash_map_t *hmap)
{
	return hmap->begin;
}
hash_map_pos_t hash_map_end(hash_map_t *hmap)
{
	return hmap->end;
}
int hash_map_insert(hash_map_t *hmap,void *data)
{
	struct hash_map_node *mnode;
	uint32_t bucket_pos;
	uint32_t data_size       = hmap->data_size;
	uint32_t key_size        = hmap->key_size;
	struct hlist_head *hlist = hmap->hlist;
	hash_func_fpt hash_func  = hmap->hash_func;
	uint32_t bucket_size     = hmap->bucket_size;;
	hash_map_pos_t *begin_pos = &hmap->begin;;

	dbg_str(DBG_DETAIL,"hash_map_insert");

	mnode = (struct hash_map_node *)mem_alloc_pck(NULL, sizeof(struct hash_map_node) + data_size, 0);
	if(mnode == NULL){
		dbg_str(DBG_ERROR,"hash_map_insert,malloc err");
		return -1;
	}

	memcpy(mnode->key,data,data_size);
	mnode->value_pos = key_size;
	mnode->data_size = data_size;

	INIT_HLIST_NODE(&mnode->hlist_node);

	bucket_pos = hash_func(mnode->key,key_size,bucket_size); 
	assert(bucket_pos <= bucket_size);

	pthread_rwlock_wrlock(&hmap->map_lock);

	hlist_add_head(&mnode->hlist_node, &hlist[bucket_pos]);
	if(begin_pos->hlist_node_p == NULL || bucket_pos <= begin_pos->bucket_pos){
		hash_map_pos_init(&hmap->begin, hlist[bucket_pos].first, bucket_pos, hlist,hmap);
		dbg_str(DBG_WARNNING,"change begin pos");
	}
	dbg_str(DBG_IMPORTANT,"bucket_pos =%d,first =%p,next=%p,begin.hash_map_pos=%p",
			bucket_pos,
			hlist[bucket_pos].first,
			hlist[bucket_pos].first->next,
			hmap->begin.hlist_node_p);
	pthread_rwlock_unlock(&hmap->map_lock);

	return 0;
}
hash_map_pos_t hash_map_search(hash_map_t *hmap, void *key)
{
	struct hash_map_node *mnode = NULL;
	uint32_t bucket_pos;
	uint32_t key_size        = hmap->key_size;
	struct hlist_head *hlist = hmap->hlist;
	hash_func_fpt hash_func  = hmap->hash_func;
	key_cmp_fpt key_cmp_func = hmap->key_cmp_func;
	uint32_t bucket_size     = hmap->bucket_size;;
	hash_map_pos_t ret;
	struct hlist_node *pos,*next;


	dbg_str(DBG_DETAIL,"hash_map_search");

	bucket_pos = hash_func(key,key_size,bucket_size); 
	assert(bucket_pos <= bucket_size);

	dbg_str(DBG_DETAIL,"hash_map_search,bucket_pos=%d",bucket_pos);
	pthread_rwlock_rdlock(&hmap->map_lock);
	hlist_for_each_safe(pos, next, &hlist[bucket_pos]){
		mnode = container_of(pos,struct hash_map_node,hlist_node);
		if(!key_cmp_func(mnode->key,key,key_size)){
			dbg_str(DBG_IMPORTANT,"found key");
			pthread_rwlock_unlock(&hmap->map_lock);
			return hash_map_pos_init(&ret, pos, bucket_pos, hlist,hmap);
		}
	}
	pthread_rwlock_unlock(&hmap->map_lock);
	return hash_map_pos_init(&ret, NULL, bucket_pos, hlist,hmap);
}
int hash_map_delete(hash_map_t *hmap, hash_map_pos_t pos)
{
	struct hash_map_node *mnode;
	struct hlist_head *hlist = pos.hlist;
	hash_map_pos_t next;

	dbg_str(DBG_IMPORTANT,"del hash_map ,bucket_pos:%d,cur node:%p,begin node:%p",
			pos.bucket_pos, pos.hlist_node_p, hmap->begin.hlist_node_p);

	pthread_rwlock_wrlock(&hmap->map_lock);
	if(hash_map_pos_equal(pos,hmap->begin)){
		dbg_str(DBG_WARNNING,"del iter equal begain");
		next = hash_map_pos_next(pos);
		hash_map_pos_init(&hmap->begin,
				next.hlist_node_p, 
				next.bucket_pos, hlist, hmap);
	}
	hlist_del(pos.hlist_node_p);
	pthread_rwlock_unlock(&hmap->map_lock);

	mnode = container_of(pos.hlist_node_p,struct hash_map_node,hlist_node);
	if (mnode != NULL) {
		free(mnode);
		mnode = NULL;
	}
	return 0;
}
int hash_map_destroy(hash_map_t *hmap)
{
	 dbg_str(DBG_DETAIL,"hash_map_destroy");
	 hash_map_pos_t it;
	 for(	 it = hash_map_begin(hmap);
			 !hash_map_pos_equal(it,hash_map_end(hmap));
			 it = hash_map_begin(hmap))
	 {
		 dbg_str(DBG_DETAIL,"destroy node:%p",it.hlist_node_p);
		 hash_map_delete(hmap,it);
	 }
	 if(hash_map_pos_equal(hmap->end,it)){
		 dbg_str(DBG_WARNNING,"hash_map_destroy,hash_map is NULL");
		 free(hmap->hlist);
	 }
}
hash_map_pos_t hash_map_pos_next(hash_map_pos_t pos)
{
	struct hlist_node *hlist_node_p;
	uint32_t bucket_pos      = pos.bucket_pos;
	struct hlist_head *hlist = pos.hlist;
	hash_map_t *hmap          = pos.hmap;
	uint32_t bucket_size     = hmap->bucket_size;

	if(pos.hlist_node_p->next){
		dbg_str(DBG_DETAIL,"find next iter");
		hlist_node_p = pos.hlist_node_p->next;
		hash_map_pos_init(&pos, hlist_node_p, bucket_pos, hlist, hmap);
		dbg_str(DBG_DETAIL,"bucket_pos=%d,node:%p,next:%p",
				bucket_pos,hlist_node_p,hlist_node_p->next);
		return pos;
	} else if(bucket_pos < bucket_size - 1 ){
		for(++bucket_pos; bucket_pos < bucket_size;bucket_pos++){
			if(hlist[bucket_pos].first){
				dbg_str(DBG_DETAIL,"find next iter");
				hlist_node_p = hlist[bucket_pos].first;
				hash_map_pos_init(&pos, hlist_node_p, bucket_pos, hlist, hmap);
				dbg_str(DBG_DETAIL,"bucket_pos=%d,node:%p,next:%p",
						bucket_pos,hlist_node_p,hlist_node_p->next);
				return pos;
			}
		}
		hash_map_pos_init(&pos, NULL, bucket_size -1, hlist, hmap);
	} else if(bucket_pos == bucket_size -1 ){
		dbg_str(DBG_IMPORTANT,"container is null");
		hash_map_pos_init(&pos, NULL, bucket_pos, hlist, hmap);
	}else{
		dbg_str(DBG_ERROR,"hash_map_iterator_next err");
	}

	return pos;
}
int hash_map_pos_equal(hash_map_pos_t pos1,hash_map_pos_t pos2)
{
	return (pos1.hlist_node_p == pos2.hlist_node_p);
}
void *hash_map_pos_get_pointer(hash_map_pos_t pos)
{
	struct hash_map_node *mnode;

	mnode = container_of(pos.hlist_node_p,
			struct hash_map_node,
			hlist_node);

	return &mnode->key[mnode->value_pos];
}
void hash_map_for_each(struct hash_map_s *hmap,void (*func)(struct hash_map_node *mnode))
{
	hash_map_pos_t pos;
	struct hash_map_node *mnode;

	for(pos = hash_map_begin(hmap); 
			!hash_map_pos_equal(pos,hash_map_end(hmap));
			pos = hash_map_pos_next(pos)){
		mnode = container_of(pos.hlist_node_p,struct hash_map_node,hlist_node);
		func(mnode);
	}
}
void hash_map_print_mnode(struct hash_map_node *mnode)
{
	dbg_buf(DBG_DETAIL,"data:",mnode->key,mnode->data_size);
}
/*
 *int main()
 *{
 *    hash_map_t *hmap;
 *    pair_t *pair;
 *    hash_map_pos_t pos;
 *    struct hash_map_node *mnode;
 *
 *    struct A t1 = {1,2};
 *    struct A t2 = {2,2};
 *    struct A t3 = {3,2};
 *    struct A t4 = {4,2};
 *    struct A t5 = {5,2};
 *
 *    pair = create_pair(2,sizeof(struct A));
 *    hash_map_init(&hmap,
 *            2,//uint32_t key_size,
 *            sizeof(struct A)+ 2,
 *            10,
 *            default_hash_func,
 *            default_key_cmp_func);
 *
 *    make_pair(pair,"11",&t1);
 *    hash_map_insert(hmap,pair->data);
 *    make_pair(pair,"22",&t2);
 *    hash_map_insert(hmap,pair->data);
 *    make_pair(pair,"33",&t3);
 *    hash_map_insert(hmap,pair->data);
 *    make_pair(pair,"44",&t4);
 *    hash_map_insert(hmap,pair->data);
 *
 *    hash_map_for_each(hmap,hash_map_print_mnode);
 *
 *    hash_map_destroy(hmap);
 *}
 */
