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

#include "libdata_structure/hash_list_struct.h"

static inline pair_t * create_pair(int key_len,int value_len)
{
	pair_t *p;
	p = (pair_t *)malloc(sizeof(pair_t) + key_len + value_len);
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

pair_t * create_pair(int key_len,int value_len);
void make_pair(pair_t *p,void *key,void *value);
int destroy_pair(pair_t *p);
int default_key_cmp_func(void *key1,void *key2,uint32_t size);
uint32_t default_hash_func(void *key,uint32_t key_size,uint32_t bucket_size);

hash_map_t * hash_map_create(allocator_t *allocator,uint8_t lock_type);
int hash_map_init(hash_map_t *hmap, uint32_t key_size, uint32_t data_size, uint32_t bucket_size, hash_func_fpt hash_func, key_cmp_fpt key_cmp_func);
hash_map_pos_t hash_map_pos_init(hash_map_pos_t *pos, struct hlist_node *hlist_node_p, uint32_t bucket_pos, struct hlist_head *hlist_head_p, struct hash_map_s *hmap);
hash_map_pos_t hash_map_begin(hash_map_t *hmap);
hash_map_pos_t hash_map_end(hash_map_t *hmap);
int hash_map_insert(hash_map_t *hmap,void *data);
hash_map_pos_t hash_map_search(hash_map_t *hmap, void *key);
int hash_map_delete(hash_map_t *hmap, hash_map_pos_t pos);
;
int hash_map_destroy(hash_map_t *hmap);
hash_map_pos_t hash_map_pos_next(hash_map_pos_t pos);
int hash_map_pos_equal(hash_map_pos_t pos1,hash_map_pos_t pos2);
void *hash_map_pos_get_pointer(hash_map_pos_t pos);
void hash_map_for_each(struct hash_map_s *hmap,void (*func)(struct hash_map_node *mnode));
void hash_map_print_mnode(struct hash_map_node *mnode);

#endif
