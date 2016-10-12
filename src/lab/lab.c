/**
 * @file lab.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-10-11
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
#include <stdio.h>
#include <libdbg/debug.h>
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

typedef struct core{
    char e;
    short f;
    int g;
    char str[50];
}__attribute__ ((packed)) core_t;
typedef struct test{
    char a;
    short b;
    int c;
    long d;
    char buf[50];
    core_t core;
}__attribute__ ((packed)) test_t;

void dump_test(test_t *t)
{
    dbg_str(DBG_DETAIL,"a =%d,b=%d,c=%d,d=%d,buf_str:%s,core.e=%d,core.f=%d,core.g=%d,core.str=%s",
            t->a,t->b,t->c,t->d,t->buf,t->core.e,t->core.f,t->core.g,t->core.str);
}

static int process_task_callback(client_task_t *task)
{
    char buf[200];
	dbg_str(DBG_DETAIL,"process_task begin,client send");
	dbg_buf(DBG_DETAIL,"task buffer:",task->buffer,task->buffer_len);
    memcpy(buf,task->buffer,task->buffer_len);
    test_t *t;

    t = (test_t *)buf;

    dump_test(t);

	dbg_str(DBG_DETAIL,"process_task end");
    return 0;
}
int lab()
{
	client_t *cli;
	const char buf[] = {1,2,3,4,5,6,7,8,9,10};
	struct sockaddr_in raddr;
	socklen_t destlen;
	allocator_t *allocator = allocator_get_default_alloc();

	cli = udp_iclient(
			allocator,
			"0.0.0.0",//char *host,
			"1989",//char *client_port,
			process_task_callback,
			NULL);
}

#if 0
int lab()
{
	client_t *cli;
	const char buf[] = {1,2,3,4,5,6,7,8,9,10};
	struct sockaddr_in raddr;
	socklen_t destlen;
	allocator_t *allocator;

	if((allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0) ) == NULL){
		dbg_str(DBG_ERROR,"proxy_create allocator_creator err");
		return -1;
	}

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

	raddr.sin_family      = AF_INET;
	raddr.sin_port        = htons(atoi("1989"));
    raddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    /*
     *inet_pton(AF_INET,"255.255.20.255",&raddr.sin_addr);
     */
    /*
     *inet_pton(AF_INET,"0.0.0.0",&raddr.sin_addr);
     */

    /*
     *int broadcast = 1;
     *if( setsockopt(cli->user_fd,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast)) == -1) {
     *    perror("setsockopt function!\n");
     *    exit(1);
     *}
     */
    udp_iclient_send(
            cli,//client_t *client,
            buf,//const void *buf,
            sizeof(buf),
			0,//int flags,
			(void *)&raddr,//const struct sockaddr *destaddr,
			sizeof(raddr));//socklen_t destlen);
}
#endif
