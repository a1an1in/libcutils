#include <stdio.h>
#include "libcontainer/inc_files.h"
#include "libcontainer/test_container.h"

int test_container_rbtree_map(void)
{
	int ret = 0;
	iterator_t it,next,end;
	container_t *ct;
	allocator_t *allocator;
	pair_t *pair;
	int key_len = 2;
	int bucket_size = 10;
	struct rbtree_map_node *mnode;
	

	allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0);
	dbg_str(DBG_CONTAINER_DETAIL,"rbtree_map allocator addr:%p",allocator);
	ct = container_creator(CONTAINER_TYPE_RBTREE_MAP,allocator,0);

	dbg_str(DBG_CONTAINER_DETAIL,"run at here");
	/*
	 *int container_rbtree_map_init(container_t *ct,
	 *        uint32_t key_size,
	 *        uint32_t value_size,
	 *        uint32_t bucket_size,
	 *        rbtree_func_ft rbtree_func)
	 */
	container_rbtree_map_init(ct, key_len, sizeof(struct test),NULL);

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
		mnode = container_of(it.pos.rbtree_pos.rb_node_p,struct rbtree_map_node,node);
		dbg_buf(DBG_CONTAINER_DETAIL,"key:",mnode->key,mnode->value_pos);
		print_test((struct test *)iterator_get_pointer(&it));
	}

	dbg_str(DBG_CONTAINER_DETAIL,"search node key = 22");
	container_map_search(ct, (void *)"22",&it);
	if(it.pos.rbtree_pos.rb_node_p){
		mnode = container_of(it.pos.rbtree_pos.rb_node_p,struct rbtree_map_node,node);
		dbg_buf(DBG_CONTAINER_DETAIL,"key:",mnode->key,mnode->value_pos);
		print_test((struct test *)iterator_get_pointer(&it));
	}
	printf("\n");
	container_destroy(&ct);
	allocator_destroy(allocator);
	return ret;
}
