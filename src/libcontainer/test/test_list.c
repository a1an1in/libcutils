#include <stdio.h>
#include "libcontainer/inc_files.h"
#include "test/test_container.h"

int test_list(void)
{
	int ret = 0;
	allocator_t *allocator;
	iterator_t it,next;
	container_t *ct;

	/*
	 *allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC);
	 */
	allocator = allocator_creator(ALLOCATOR_TYPE_CDS_MALLOC);
	allocator_ctr_init(allocator, 0, 0, 1024);

	dbg_str(DBG_CONTAINER_DETAIL,"list allocator addr:%p",allocator);

	ct = container_creator(CONTAINER_TYPE_LIST,allocator);

	container_list_init(ct,sizeof(struct test));
	/*
	 *container_push_front(ct,&test);
	 *container_push_front(ct,&test2);
	 *container_push_front(ct,&test3);
	 */
	dbg_str(DBG_CONTAINER_DETAIL,"run at here");
	container_push_back(ct,&test);
	container_push_back(ct,&test2);
	container_push_back(ct,&test3);


	dbg_str(DBG_CONTAINER_DETAIL,"iter ordinal");
	for(	it = container_begin(ct); 
			!iterator_equal(it,container_end(ct));
			it = iterator_next(it))
	{
		print_test((struct test *)iterator_get_pointer(it));
	}

	dbg_str(DBG_CONTAINER_DETAIL,"iter insert test");
	int i = 0;
	for(	it = container_begin(ct); 
			!iterator_equal(it,container_end(ct));
			it = iterator_next(it),i++)
	{
		if(i == 1){
			break;
		}
	}
	dbg_str(DBG_CONTAINER_DETAIL,"it list_head:%p",it.pos.list_head_p);
	container_insert(ct,it,&test4);

	dbg_str(DBG_CONTAINER_DETAIL,"iter delte test");
	container_for_each_safe(it,next,ct){
		//container_delete(ct,it);
		print_test((struct test *)iterator_get_pointer(it));
	}

	dbg_str(DBG_CONTAINER_DETAIL,"inquire alloc info");
	allocator_mem_info(allocator);

	container_destroy(&ct);
	allocator_destroy(allocator);
	dbg_str(DBG_CONTAINER_DETAIL,"ct addr =%p",ct);
	/*
	 *it = container_begin(ct); 
	 *for(	it = container_begin(ct); 
	 *        !iterator_equal(it,container_end(ct));
	 *        it = iterator_next(it))
	 *{
	 *    dbg_str(DBG_CONTAINER_DETAIL,"run at here");
	 *    print_test((struct test *)iterator_get_pointer(it));
	 *}
	 */
	/*
	 *dbg_str(DBG_CONTAINER_DETAIL,"iter pop back test");
	 *container_pop_back(ct);
	 *for(	it = container_begin(ct); 
	 *        !iterator_equal(it,container_end(ct));
	 *        it = iterator_next(it))
	 *{
	 *    print_test((struct test *)iterator_get_pointer(it));
	 *}
	 */

	/*
	 *dbg_str(DBG_CONTAINER_DETAIL,"iter pop front test");
	 *container_pop_front(ct);
	 *for(	it = container_begin(ct); 
	 *        !iterator_equal(it,container_end(ct));
	 *        it = iterator_next(it))
	 *{
	 *    print_test((struct test *)iterator_get_pointer(it));
	 *}
	 */
	/*
	 *dbg_str(DBG_CONTAINER_DETAIL,"iter reverse");
	 *for(	it = iterator_prev(container_end(ct)); 
	 *        iterator_equal(it,container_end(ct));
	 *        it = iterator_prev(it))
	 *{
	 *    print_test((struct test *)iterator_get_pointer(it));
	 *}
	 */
	return ret;
}
