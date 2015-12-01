/*
 * =====================================================================================
 *
 *       Filename:  test_hashmap.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/14/2015 10:07:09 AM
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
#include "libdata_structure/hash_list.h"
#include "liballoc/allocator.h"

struct A{
	int a;
	int b;
};
void test_datastructure_hashlist()
{
	hash_map_t *hmap;
	pair_t *pair;
	hash_map_pos_t pos;
	struct hash_map_node *mnode;
	allocator_t *allocator;

	struct A t1 = {1,2};
	struct A t2 = {2,2};
	struct A t3 = {3,2};
	struct A t4 = {4,2};
	struct A t5 = {5,2};

	allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0);

	pair = create_pair(2,sizeof(struct A));

	hmap = hash_map_create(allocator,0);
	hash_map_init(hmap,
			2,//uint32_t key_size,
			sizeof(struct A)+ 2,
			10,
			default_hash_func,
			default_key_cmp_func);

	make_pair(pair,"11",&t1);
	hash_map_insert(hmap,pair->data);
	make_pair(pair,"22",&t2);
	hash_map_insert(hmap,pair->data);
	make_pair(pair,"33",&t3);
	hash_map_insert(hmap,pair->data);
	make_pair(pair,"55",&t4);
	hash_map_insert(hmap,pair->data);

	dbg_str(DBG_DETAIL,"hash map search");
	struct A *p;
	hash_map_search(hmap,"33",&pos);
	p = hash_map_pos_get_pointer(&pos);
	dbg_str(DBG_DETAIL,"a =%d, b=%d",p->a,p->b);

	dbg_str(DBG_DETAIL,"for each");
	hash_map_for_each(hmap,hash_map_print_mnode);

	hash_map_destroy(hmap);
}
