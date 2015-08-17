/*
 * =====================================================================================
 *
 *       Filename:  hash_list.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/14/2015 09:00:22 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __HASH_LIST_ST_H__
#define __HASH_LIST_ST_H__

#include <pthread.h>
#include "libdata_structure/list.h"
#include "liballoc/allocator.h"

/*
 *typedef unsigned int uint32_t;
 *typedef unsigned short uint16_t;
 *typedef unsigned char uint8_t;
 */

typedef struct pair{
	uint16_t data_len;
	uint32_t key_len;
	uint32_t value_len;
	uint8_t data[1];
}pair_t;

#ifndef __KEY_CMP_FPT__
#define __KEY_CMP_FPT__
typedef int (*key_cmp_fpt)(void *key1,void *key2,uint32_t key_size);
#endif
typedef uint32_t (*hash_func_fpt)(void *key,uint32_t key_size,uint32_t bucket_size);

struct hash_map_node {
	struct hlist_node hlist_node;
	uint8_t value_pos;//value's pos at key array
	uint32_t data_size;
	char key[1];
};
typedef struct hash_map_pos_info{
	struct hlist_node *hlist_node_p; //node addr
	uint32_t bucket_pos;//bucket pos
	struct hlist_head *hlist;//hash table head addr
	struct hash_map_s *hmap;
}hash_map_pos_t;

typedef struct hash_map_s{
	uint32_t bucket_size;
	uint32_t data_size;
	uint32_t key_size;
	hash_func_fpt hash_func;
	key_cmp_fpt key_cmp_func;
	struct hlist_head *hlist;
	hash_map_pos_t begin,end;
	pthread_rwlock_t map_lock;
	allocator_t *allocator;
}hash_map_t;

#endif
