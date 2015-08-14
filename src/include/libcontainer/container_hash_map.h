/*
 * =====================================================================================
 *
 *       Filename:  container_hash_map.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/10/2015 10:30:59 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include "list.h"
#include "basic_types.h"

#ifndef __KEY_CMP_FPT__
#define __KEY_CMP_FPT__
typedef int (*key_cmp_fpt)(void *key1,void *key2,uint32_t key_size);
#endif
typedef uint32_t (*hash_func_fpt)(void *key,uint32_t key_size,uint32_t bucket_size);

struct hash_map_node {
	struct hlist_node hlist_node;
	uint8_t value_pos;//value's pos at key array
  	char key[1];
};
typedef struct hash_map_pos_info{
	struct hlist_node *hlist_node_p; //node addr
	uint32_t bucket_pos;//bucket pos
	struct hlist_head *hlist;//hash table head addr
}hash_map_pos_t;

typedef struct hash_map_priv_info{
	uint32_t bucket_size;
	uint32_t data_size;
	uint32_t key_size;
	hash_func_fpt hash_func;
	key_cmp_fpt key_cmp_fpt;
}hash_map_priv_t;

int container_hash_map_register();
#endif
