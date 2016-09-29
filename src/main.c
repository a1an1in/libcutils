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

#define LIBRARY_VERSION "libcutils version 2.1.1.0"
void test_datastructure()
{
    test_datastructure_hashlist();
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

#ifndef MAKELIB

typedef struct config_list_s{
}config_list_t;

typedef struct base_s{
	config_list_t config;
	args_processor_t *p;
}base_t;

static int args_process_test_state_machine(void *base,int argc,char **argv)
{
    test_state_machine();
	return 1;
}
static int args_process_test_udp_unix_client_recieve(void *base,int argc,char **argv)
{
    test_udp_unix_client_recieve();
	return 1;
}
static int args_process_test_udp_unix_client_send(void *base,int argc,char **argv)
{
    test_udp_unix_client_send();
	return 1;
}
static int args_process_test_tcp_unix_client_send(void *base,int argc,char **argv)
{
    test_tcp_unix_client_send();
	return 1;
}
static int args_process_test_tcp_unix_server(void *base,int argc,char **argv)
{
    test_tcp_unix_server();
	return 1;
}
static int args_process_test_share_mem_read(void *base,int argc,char **argv)
{
    test_share_mem_read();
	return 1;
}
static int args_process_test_share_mem_write(void *base,int argc,char **argv)
{
    test_share_mem_write();
	return 1;
}
static int args_process_port(void *base,int argc,char **argv)
{
	dbg_str(DBG_DETAIL,"args_process_port:%s",argv[0]);
	return 1;
}
static int args_process_ipaddr(void *base,int argc,char **argv)
{
	dbg_str(DBG_DETAIL,"args_process_ipaddr:%s",argv[0]);
	return 1;
}
static int args_process_help(void *base,int argc,char **argv)
{
	args_print_help_info(args_get_processor_globle_addr());
    exit(1);
	return 0;
}

static cmd_config_t cmds[]={
	{"test_state_machine", args_process_test_state_machine,0, "test_state_machine", "N/A",""},
	{"test_udp_unix_client_recieve", args_process_test_udp_unix_client_recieve,0, "test_udp_unix_client_recieve", "N/A",""},
	{"test_udp_unix_client_send", args_process_test_udp_unix_client_send,0, "test_udp_unix_client_send", "N/A",""},
	{"test_tcp_unix_client_send", args_process_test_tcp_unix_client_send,0, "test_tcp_unix_client_send", "N/A",""},
	{"test_tcp_unix_server", args_process_test_tcp_unix_server,0, "test_tcp_unix_server", "N/A",""},
	{"test_shm_read", args_process_test_share_mem_read,0, "test_shm_read", "N/A",""},
	{"test_shm_write", args_process_test_share_mem_write,0, "test_shm_write", "N/A",""},
	{"port", args_process_port,1, "port", "NN(number)","udp port,using to send/rcv msg with neighbor"},
	{"ip", args_process_ipaddr,1, "ip addr", "xx.xx.xx.xx","ip addr,using to send/rcv msg with neighbor"},
	{"help", args_process_help,0, "help", "help","help info"},
	{NULL,NULL,0,NULL,NULL,NULL},
};

/*
 * The libs used modules has been registered before main func,
 * and debugger has been construct before main too. so can use
 * it derectly.
 */
int main(int argc, char *argv[])
{
	int ret = 0;

    printf("\n\n");
	dbg_str(DBG_DETAIL,"test begin");

	args_process(NULL,cmds,argc, argv);
    /*
	 *test_libargs(argc, agrv);
     */
	test_allocator();
	test_datastructure();
	test_analyzer();
	test_libthread_pool();
	test_libconcurrent();
	test_libnet();
	test_libevent_tiny();

	dbg_str(DBG_DETAIL,"test end");
	pause();
	dbg_str(DBG_DETAIL,"test end,but pause breaked");

	return ret;
}
#endif

__attribute__((constructor(101))) void
print_library_version()
{
    printf("%s\n",LIBRARY_VERSION);
}
