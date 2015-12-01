/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/19/2015 5:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
/*  
 * Copyright (c) 2015-2020 alan lin <a1an1in@sina.com>
 *  
 *  
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <stdio.h>

#include "libdbg/debug.h"
#include "libdbg/libdbg_register_modules.h"
#include "liballoc/liballoc_register_modules.h"
#include "libcontainer/container.h"
#include "libcre/libcre_register_modules.h"

#include "libcontainer/test_container.h"
#include "libdata_structure/test_datastructure.h"
#include "libproto_analyzer/test_proto_analyzer.h"

#define LIBCDF_VERSION "1.2.1.0"

debugger_t *debugger_gp;

void register_all_lib_modules()
{
	libdbg_register_modules();
	liballoc_register_modules();
	libcontainer_register_modules();
	libcre_register_modules();
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
	/*
	 *test_datastructure_link_list();
	 */
}
void test_analyzer()
{
	test_pdt_proto_analyzer();
	/*
	 *test_pdu_proto_analyzer();
	 */
}
int main()
{
	int ret = 0;

	printf("LIBCDF_VERSION:%s\n",LIBCDF_VERSION);
	register_all_lib_modules();

	debugger_gp = debugger_creator("dbg.ini",0);
	debugger_init(debugger_gp);

	dbg_str(DBG_DETAIL,"debugger is start up");

	test_container();
	test_datastructure();
	test_analyzer();

	dbg_str(DBG_DETAIL,"test end");

	return ret;
}
