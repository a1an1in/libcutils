#include <stdio.h>
#include "libcontainer/inc_files.h"
#include "libcontainer/test_container.h"

int test_rbtree_map(void)
{
	int ret = 0;
	iterator_t it,next;
	container_t *ct;
	allocator_t *allocator;
	pair_t *pair;
	int key_len = 3;
	struct rbtree_map_node *mnode;


	allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0);
	dbg_str(DBG_CONTAINER_DETAIL,"rbtree allocator addr:%p",allocator);
	ct = container_creator(CONTAINER_TYPE_RBTREE_MAP,allocator,0);

	container_tree_map_init(ct,key_len,sizeof(struct test),NULL);

	dbg_str(DBG_CONTAINER_DETAIL,"create_pair");
	pair = create_pair(key_len,sizeof(struct test));

	make_pair(pair,(void *)"ebf",(void *)&test);
	container_map_insert(ct, pair);
	make_pair(pair,(void *)"abc",(void *)&test3);
	container_map_insert(ct, pair);
	make_pair(pair,(void *)"zge",(void *)&test2);
	container_map_insert(ct, pair);
	make_pair(pair,(void *)"1bf",(void *)&test);
	container_map_insert(ct, pair);
	make_pair(pair,(void *)"2bc",(void *)&test3);
	container_map_insert(ct, pair);
	make_pair(pair,(void *)"3ge",(void *)&test2);
	container_map_insert(ct, pair);

	dbg_str(DBG_CONTAINER_DETAIL,"foreach ordinal print");
	for(	it = container_begin(ct); 
			!iterator_equal(it,container_end(ct));
			it = iterator_next(it))
	{
		mnode = rb_entry(it.pos.rb_node_p,struct rbtree_map_node,node);
		dbg_buf(DBG_CONTAINER_DETAIL,"key:",mnode->key,mnode->value_pos);
		print_test((struct test *)iterator_get_pointer(it));
	}

	dbg_str(DBG_CONTAINER_DETAIL,"search node key = zge");
	it = container_map_search(ct, (void *)"zge");
	print_test((struct test *)iterator_get_pointer(it));

	dbg_str(DBG_CONTAINER_DETAIL,"delte node key = zge");
	container_delete(ct,it);

	/*
	 *dbg_str(DBG_CONTAINER_DETAIL,"container_for_each delte node");
	 *for(	it = container_begin(ct); 
	 *        !iterator_equal(it,container_end(ct));
	 *        it = container_begin(ct)) 
	 *{
	 *    container_delete(ct,it);
	 *    print_test((struct test *)iterator_get_pointer(it));
	 *}
	 */
	container_destroy(&ct);
	allocator_destroy(allocator);
	dbg_str(DBG_CONTAINER_DETAIL,"ct addr =%p",ct);
	/*
	 *printf("\n");
	 *dbg_str(DBG_CONTAINER_DETAIL,"iter ordinal");
	 *for(	it = container_begin(ct); 
	 *        !iterator_equal(it,container_end(ct));
	 *        it = iterator_next(it))
	 *{
	 *    print_test((struct test *)iterator_get_pointer(it));
	 *}
	 */
	return ret;
}

