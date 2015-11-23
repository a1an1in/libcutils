/*
 * =====================================================================================
 *
 *       Filename:  hash_list.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/14/2015 09:06:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __HASH_LIST_H__
#define __HASH_LIST_H__

#include <libdbg/debug.h>
#include "libdata_structure/hash_list_struct.h"

hash_map_t * hash_map_create(allocator_t *allocator,uint8_t lock_type);
int hash_map_insert(hash_map_t *hmap,void *data);
hash_map_pos_t hash_map_search(hash_map_t *hmap, void *key);
int hash_map_delete(hash_map_t *hmap, hash_map_pos_t pos);
int hash_map_destroy(hash_map_t *hmap);
hash_map_pos_t hash_map_pos_next(hash_map_pos_t pos);
void hash_map_print_mnode(struct hash_map_node *mnode);

static inline pair_t * create_pair(int key_len,int value_len)
{
	pair_t *p;
	p = (pair_t *)malloc(sizeof(pair_t) + key_len + value_len);
	memset(p,0,sizeof(pair_t) + key_len + value_len);
	p->key_len = key_len;
	p->value_len = value_len;
	p->data_len = key_len + value_len;
	return p;
}
static inline void make_pair(pair_t *p,void *key,void *value)
{
	memcpy(p->data,key,p->key_len);
	memcpy(p->data + p->key_len,value,p->value_len);
}
static inline int destroy_pair(pair_t *p)
{
	free(p);
	p = NULL;
	return 0;
}

static inline int default_key_cmp_func(void *key1,void *key2,uint32_t size)
{
	return memcmp(key1,key2,size);
}
static inline uint32_t default_hash_func(void *key,uint32_t key_size,uint32_t bucket_size)
{
	uint32_t sum = 0;
	uint32_t i = 0;
	for(i = 0; i < key_size; i++){
		sum += *(uint8_t *)(key + i);
	}
	return sum % bucket_size;
}

static inline hash_map_pos_t hash_map_begin(hash_map_t *hmap)
{
	return hmap->begin;
}
static inline hash_map_pos_t hash_map_end(hash_map_t *hmap)
{
	return hmap->end;
}
static inline hash_map_pos_t hash_map_pos_init(hash_map_pos_t *pos,
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
static inline int hash_map_pos_equal(hash_map_pos_t pos1,hash_map_pos_t pos2)
{
	return (pos1.hlist_node_p == pos2.hlist_node_p);
}
static inline void *hash_map_pos_get_pointer(hash_map_pos_t pos)
{
	struct hash_map_node *mnode;

	mnode = container_of(pos.hlist_node_p,
			struct hash_map_node,
			hlist_node);
	dbg_buf(DBG_DETAIL,"key:",mnode->key,mnode->data_size);

	return &mnode->key[mnode->value_pos];
}
static inline void hash_map_for_each(struct hash_map_s *hmap,void (*func)(struct hash_map_node *mnode))
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

#endif
