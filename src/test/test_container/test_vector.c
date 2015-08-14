#include <stdio.h>
#include "libcontainer/inc_files.h"
#include "test/test_container.h"

int test_vector(void)
{
	int ret = 0;
	allocator_t *allocator;
	iterator_t it,next;
	container_t *ct;

	allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC);
	dbg_str(DBG_CONTAINER_DETAIL,"vector allocator addr:%p",allocator);
	ct = container_creator(CONTAINER_TYPE_VECTOR,allocator);

	container_vector_init(ct,sizeof(struct test),6);

	container_push_back(ct,&test);
	container_push_back(ct,&test2);
	container_push_back(ct,&test3);
	container_push_back(ct,&test);
	container_push_back(ct,&test2);
	container_push_back(ct,&test3);
	container_push_back(ct,&test);
	container_push_back(ct,&test2);
	container_push_back(ct,&test3);

	printf("\n");
	dbg_str(DBG_CONTAINER_DETAIL,"iter ordinal");
	for(	it = container_begin(ct); 
			!iterator_equal(it,container_end(ct));
			it = iterator_next(it))
	{
		//dbg_str(DBG_CONTAINER_DETAIL,"it_pos:%d",it.pos.vector_pos);
		print_test((struct test *)iterator_get_pointer(it));
	}



	printf("\n");
	dbg_str(DBG_CONTAINER_DETAIL,"insert  test");
	int i = 0;
	for(	it = container_begin(ct); 
			!iterator_equal(it,container_end(ct));
			it = iterator_next(it))
	{
		if(i == 3){
			container_insert(ct,it,&test4);
			break;
		}
		i++;
	}
	dbg_str(DBG_CONTAINER_DETAIL,"iter ordinal");
	for(	it = container_begin(ct); 
			!iterator_equal(it,container_end(ct));
			it = iterator_next(it))
	{
		//dbg_str(DBG_CONTAINER_DETAIL,"it_pos:%d",it.pos.vector_pos);
		print_test((struct test *)iterator_get_pointer(it));
	}





	printf("\n");
	dbg_str(DBG_CONTAINER_DETAIL,"container_delete test");
	i = 0;
	for(	it = container_begin(ct); 
			!iterator_equal(it,container_end(ct));
			it = iterator_next(it))
	{
		if(i == 3){
			container_delete(ct,it);
			break;
		}
		i++;
	}
	dbg_str(DBG_CONTAINER_DETAIL,"iter ordinal");
	for(	it = container_begin(ct); 
			!iterator_equal(it,container_end(ct));
			it = iterator_next(it))
	{
		//dbg_str(DBG_CONTAINER_DETAIL,"it_pos:%d",it.pos.vector_pos);
		print_test((struct test *)iterator_get_pointer(it));
	}




	printf("\n");
	dbg_str(DBG_CONTAINER_DETAIL,"back iter ordinal");
	for(	it = container_end(ct),it = iterator_prev(it); 
			!iterator_equal(container_begin(ct),container_end(ct));
			it = container_end(ct))
	{
		print_test((struct test *)iterator_get_pointer(it));
		container_pop_back(ct);
	}
	/*
	 *dbg_str(DBG_CONTAINER_DETAIL,"it vector_head:%p",it.pos.vector_head_p);
	 *container_insert(ct,it,&test4);
	 *container_for_each_safe(it,next,ct){
	 *    print_test((struct test *)iterator_get_pointer(it));
	 *}
	 */

	container_destroy(&ct);
	allocator_destroy(allocator);

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
