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
#include <unistd.h>
#include <libdbg/debug.h>
#include <libnet/client.h>
#include <libargs/cmd_args.h>
#include <libstate_machine/state_machine.h>

#define LIBRARY_VERSION "libcutils version 2.0.0.1"
void test_datastructure()
{
	/*
	 *test_datastructure_hashlist();
	 */
	/*
	 *test_datastructure_link_list();
	 */
	/*
	 *test_datastructure_rbtree_map();
	 */
	/*
	 *test_datastructure_vector();
	 */
	 
}
void test_analyzer()
{
	/*
	 *test_pdt_proto_analyzer();
	 */
	/*
	 *test_pdu_proto_analyzer();
	 */
}
void test_allocator()
{
	/*
	 *test_ctr_alloc();
	 */
}
void test_libthread_pool()
{
	/*
	 *test_shutdown();
	 */
	/*
	 *test_thrdtest();
	 */
}
void test_libconcurrent()
{
    /*
     *test_concurrent3();
     */
    /*
     *test_tmr_user();
     */
    /*
     *test_io_user();
     */
}
void test_libnet()
{
    /*
     *test_server();
     */
    /*
     *test_udp_client_recieve();
     */
    /*
     *test_udp_client_send();
     */
    /*
     *test_tcp_client_send();
     */
}
int test_libevent_tiny()
{
    /*
     *test_io();
     */
	/*
	 *test_signal();
	 */
	/*
	 *test_signal2();
	 */
	/*
	 *test_time();
	 */
	/*
	 *test_time2();
	 */
}
void test_libargs(int argc, char *argv[])
{
	test_args(argc,argv);
}

void test_libstate_machine()
{
    /*
     *test_state_machine();
     */
}
void test_libipc()
{
    /*
     *test_udp_unix_client_recieve();
     */
    /*
     *test_udp_unix_client_send();
     */
    test_tcp_unix_client_send();
    /*
     *test_tcp_unix_server();
     */
}
#ifndef MAKELIB

/*
 * The libs used modules has been registered before main func,
 * and debugger has been construct before main too. so can use
 * it derectly.
 */
int main(int argc, char *agrv[])
{
	int ret = 0;

    printf("\n\n");
	dbg_str(DBG_DETAIL,"test begin");

	test_libargs(argc, agrv);
	test_allocator();
	test_datastructure();
	test_analyzer();
	test_libthread_pool();
	test_libconcurrent();
	test_libnet();
	test_libevent_tiny();
    test_libstate_machine();
    test_libipc();

	dbg_str(DBG_DETAIL,"test end");
	pause();
	dbg_str(DBG_DETAIL,"test end,but pause breaked");

	return ret;
}
#endif

__attribute__((constructor(112))) void
print_library_version()
{
    dbg_str(DBG_DETAIL,"%s",LIBRARY_VERSION);
}
