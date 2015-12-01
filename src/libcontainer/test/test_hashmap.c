#include <stdio.h>
#include "libcontainer/inc_files.h"
#include "libcontainer/test_container.h"

int test_hash_map(void)
{
	int ret = 0;
	iterator_t it,next,end;
	container_t *ct;
	allocator_t *allocator;
	pair_t *pair;
	int key_len = 2;
	int bucket_size = 10;
	struct hash_map_node *mnode;
	

	allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0);
	dbg_str(DBG_CONTAINER_DETAIL,"hash_map allocator addr:%p",allocator);
	ct = container_creator(CONTAINER_TYPE_HASH_MAP,allocator,0);

	dbg_str(DBG_CONTAINER_DETAIL,"run at here");
	/*
	 *int container_hash_map_init(container_t *ct,
	 *        uint32_t key_size,
	 *        uint32_t value_size,
	 *        uint32_t bucket_size,
	 *        hash_func_ft hash_func)
	 */
	container_hash_map_init(ct, key_len, sizeof(struct test),
			bucket_size, NULL,NULL);

	dbg_str(DBG_CONTAINER_DETAIL,"create_pair");
	pair = create_pair(key_len,sizeof(struct test));

	make_pair(pair,(void *)"11",(void *)&test);
	container_map_insert(ct, pair);
	make_pair(pair,(void *)"22",(void *)&test3);
	container_map_insert(ct, pair);
	make_pair(pair,(void *)"23",(void *)&test4);
	container_map_insert(ct, pair);
	make_pair(pair,(void *)"34",(void *)&test2);
	container_map_insert(ct, pair);
	/*
	 *make_pair(pair,(void *)"ad",(void *)&test);
	 *container_map_insert(ct, pair);
	 *make_pair(pair,(void *)"b4",(void *)&test4);
	 *container_map_insert(ct, pair);
	 *make_pair(pair,(void *)"c3",(void *)&test4);
	 *container_map_insert(ct, pair);
	 *make_pair(pair,(void *)"od",(void *)&test2);
	 *container_map_insert(ct, pair);
	 *make_pair(pair,(void *)"64",(void *)&test2);
	 *container_map_insert(ct, pair);
	 *make_pair(pair,(void *)"9d",(void *)&test);
	 *container_map_insert(ct, pair);
	 *make_pair(pair,(void *)"t4",(void *)&test4);
	 *container_map_insert(ct, pair);
	 */

	printf("\n");
	dbg_str(DBG_CONTAINER_DETAIL,"get data test");
	/*
	 *it = container_begin(ct); 
	 *print_test((struct test *)iterator_get_pointer(it));
	 *it = iterator_next(it);
	 *print_test((struct test *)iterator_get_pointer(it));
	 *it = iterator_next(it);
	 *print_test((struct test *)iterator_get_pointer(it));
	 */

	dbg_str(DBG_CONTAINER_DETAIL,"foreach ordinal print");
	for(	container_begin(ct,&it),iterator_next(&it,&next); 
			!iterator_equal(&it,container_end(ct,&end));
			it = next,iterator_next(&it,&next))
	{
		dbg_str(DBG_CONTAINER_DETAIL,"cur=%p next=%p",it.pos.hash_pos.hlist_node_p,it.pos.hash_pos.hlist_node_p->next);
		mnode = container_of(it.pos.hash_pos.hlist_node_p,struct hash_map_node,hlist_node);
		dbg_buf(DBG_CONTAINER_DETAIL,"key:",mnode->key,mnode->value_pos);
		print_test((struct test *)iterator_get_pointer(&it));
	}

	dbg_str(DBG_CONTAINER_DETAIL,"search node key = 22");
	container_map_search(ct, (void *)"22",&it);
	if(it.pos.hash_pos.hlist_node_p){
		dbg_str(DBG_CONTAINER_DETAIL,"bocket pos:%d",it.pos.hash_pos.bucket_pos);
		mnode = container_of(it.pos.hash_pos.hlist_node_p,struct hash_map_node,hlist_node);
		dbg_buf(DBG_CONTAINER_DETAIL,"key:",mnode->key,mnode->value_pos);
		print_test((struct test *)iterator_get_pointer(&it));
	}
	printf("\n");
	container_destroy(&ct);
	allocator_destroy(allocator);
	return ret;
}
