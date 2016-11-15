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
#include <libipc/net/inet_client.h>


static int process_task_callback(client_task_t *task)
{
	dbg_str(DBG_DETAIL,"process_task begin,client send recv data");
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
int test_udp_iclient_send()
{
	client_t *cli;
	const char buf[] = {1,2,3,4,5,6,7,8,9,10};
	struct sockaddr_in raddr;
	socklen_t destlen;
	allocator_t *allocator = allocator_get_default_alloc();

	/*
	 *proxy_constructor();
	 *sleep(2);
	 */

	cli = udp_iclient(
			allocator,
			"192.168.20.26",//char *host,
			"2016",//char *client_port,
			process_task_callback,
			NULL);
	raddr.sin_family = AF_INET; 
	raddr.sin_port = htons(atoi("1989"));  
	inet_pton(AF_INET,"0.0.0.0",&raddr.sin_addr);

	__udp_iclient_send(
			cli,//client_t *client,
			buf,//const void *buf,
			sizeof(buf),
			0,//int flags,
			(void *)&raddr,//const struct sockaddr *destaddr,
			sizeof(raddr));//socklen_t destlen);
}
int test_tcp_iclient_send()
{
	client_t *cli;
	const char buf[] = {1,2,3,4,5,6,7,8,9,10};
	struct sockaddr_in raddr;
	socklen_t destlen;
	allocator_t *allocator = allocator_get_default_alloc();

	/*
	 *proxy_constructor();
	 *sleep(2);
	 */

	cli = tcp_iclient(
			allocator,
			"127.0.0.1",//char *host,
			"6888",//char *client_port,
			process_task_callback,
			NULL);

	raddr.sin_family = AF_INET; 
	raddr.sin_port = htons(atoi("6888"));  
	inet_pton(AF_INET,"0.0.0.0",&raddr.sin_addr);

	tcp_iclient_send(
			cli,//client_t *client,
			buf,//const void *buf,
			sizeof(buf),0);//socklen_t destlen);
}
