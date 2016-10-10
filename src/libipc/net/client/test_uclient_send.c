/**
 * @file test_client_send.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 1.0
 * @date 2016-09-04
 */

/* Copyright (c) 2015-2020 alan lin <a1an1in@sina.com>
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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>     /* basic system data types */
#include <sys/socket.h>    /* basic socket definitions */
#include <netinet/in.h>    /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>     /* inet(3) functions */
#include <sys/epoll.h>     /* epoll function */
#include <fcntl.h>         /* nonblocking */
#include <sys/resource.h>  /*setrlimit */
#include <signal.h>
#include <libconcurrent/concurrent.h>
#include <libipc/net/unix_client.h>
#include <sys/un.h>


static int process_task_callback(client_task_t *task)
{
	dbg_str(DBG_DETAIL,"process_task begin,client send");
	dbg_buf(DBG_DETAIL,"task buffer:",task->buffer,task->buffer_len);
	dbg_str(DBG_DETAIL,"process_task end");
}
/*
 *enum socktype_e{
 *    SOCKTYPE_UDP = 0,
 *    SOCKTYPE_TCP,
 *    SOCKTYPE_UNIX
 *};
 */
int test_udp_uclient_send()
{
	client_t *cli;
	const char buf[] = {1,2,3,4,5,6,7,8,9,10};
	allocator_t *allocator;

	if((allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0) ) == NULL){
		dbg_str(DBG_ERROR,"proxy_create allocator_creator err");
		return -1;
	}

	cli = udp_uclient(
			allocator,
			"test_server_unix_path",//char *host,
			process_task_callback,
			NULL);

	udp_uclient_send(
			cli,//client_t *client,
			buf,//const void *buf,
			sizeof(buf),
			0,//int flags,
			"test_client_unix_path");//socklen_t destlen);

    sleep(5);
    uclient_destroy(cli);
}
int test_tcp_uclient_send()
{
	client_t *cli;
	const char buf[] = {1,2,3,4,5,6,7,8,9,10};
	struct sockaddr_in raddr;
	socklen_t destlen;
	allocator_t *allocator = allocator_get_default_alloc();

	cli = tcp_uclient(
			allocator,
            "test_server_un_path",
			process_task_callback,
			NULL);

	tcp_uclient_send(
			cli,//client_t *client,
			buf,//const void *buf,
			sizeof(buf),0);//socklen_t destlen);

    sleep(5);

    uclient_destroy(cli);

}
