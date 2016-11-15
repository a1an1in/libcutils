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
#if 0
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

#else
int lab()
{
	client_t *cli;
	char buf[] = {1,2,3,4,5,6,7,8,9,10};
	struct sockaddr_in raddr;
	socklen_t destlen;
	allocator_t *allocator = allocator_get_default_alloc();


	cli = udp_iclient(allocator,
			          "192.168.20.97",//char *host,
			          "1989",//char *client_port,
			          process_task_callback,
			          NULL);

    udp_iclient_broadcast(cli,(char *)"1989",buf,sizeof(buf));

    /*
     *udp_iclient_send(cli, buf,sizeof(buf), 0, "192.168.20.128", "1989");
     */
}
#endif
