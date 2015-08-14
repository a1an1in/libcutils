#include <stdio.h>
#include "libdbg/debug.h"
#include "libdbg/libdbg_register_modules.h"
#include "liballoc/liballoc_register_modules.h"
#include "libcontainer/libcontainer_register_modules.h"
#include "libcontainer/test_container.h"
#include "libdatastructure/test_datastructure.h"

debugger_t *debugger_gp;

void register_all_lib_modules()
{
	libdbg_register_modules();
	liballoc_register_modules();
	libcontainer_register_modules();
}
void test_container()
{
	/*
	 *test_list();
	 */
	/*
	 *dbg_str(DBG_DETAIL,"test_rbtree_map");
	 *test_rbtree_map();
	 */
	/*
	 *dbg_str(DBG_DETAIL,"test_hash_map");
	 *test_hash_map();
	 */
	/*
	 *dbg_str(DBG_DETAIL,"test_vector");
	 *test_vector();
	 */
	/*
	 *dbg_str(DBG_DETAIL,"test_cds_alloc");
	 *test_ctr_alloc();
	 */
}
void test_datastructure()
{
	/*
	 *test_datastructure_hashlist();
	 */
	test_datastructure_link_list();
}
int main()
{
	int ret = 0;

	register_all_lib_modules();

	debugger_gp = debugger_creator("dbg.ini");
	debugger_init(debugger_gp);


	dbg_str(DBG_DETAIL,"test begin");

	test_container();
	test_datastructure();

	dbg_str(DBG_DETAIL,"test end");

	return ret;
}
