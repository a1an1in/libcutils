/*
 * =====================================================================================
 *
 *       Filename:  test_datastructure_link_list.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/14/2015 11:16:44 AM
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
#include "libdbg/debug.h"
#include "liballoc/allocator.h"
#include "libdata_structure/link_list.h"

struct test{
	int a;
	int b;
};
void print_list_data(list_t *list)
{

	struct test *t = (struct test *)list->data;

	dbg_str(DBG_DETAIL,"a=%d,b=%d",t->a,t->b);
}
int test_datastructure_link_list()
{
	llist_t *llist;
	allocator_t *allocator;

	struct test t1={1,2};
	struct test t2={2,2};
	struct test t3={3,2};
	struct test t4={4,2};
	int ret = 0;

	/*
	 *allocator = allocator_creator(ALLOCATOR_TYPE_CTR_MALLOC);
	 *allocator_ctr_init(allocator, 0, 0, 1024);
	 *dbg_str(DBG_CONTAINER_DETAIL,"list allocator addr:%p",allocator);
	 */

	allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0);

	//ct = container_creator(CONTAINER_TYPE_LIST,allocator);
	llist = llist_create(allocator,0);
	llist_init(llist,sizeof(struct test));
	llist_push_front(llist,&t1);
	llist_push_front(llist,&t2);
	llist_push_front(llist,&t3);
	llist_push_front(llist,&t4);

	llist_for_each(llist,print_list_data);

	llist_destroy(llist);
	return ret;
}
