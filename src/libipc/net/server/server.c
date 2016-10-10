/**
 * @file server.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-10-09
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
#include <libipc/net/inet_server.h>
#include <libipc/net/unix_server.h>
#include <libipc/net/server.h>
#include <signal.h>

server_t * server(allocator_t *allocator,
                  char *type,
                  char *host_ip, 
                  char *server_port,
                  int (*process_task_cb)(void *task),
                  void *opaque)
{
    server_t *s;

    if(!strcmp(type,TCP_ISERVER_TYPE)){
        s = tcp_iserver(allocator, host_ip, server_port, 
                process_task_cb, opaque);
    } else if (!strcmp(type,TCP_USERVER_TYPE)){
        s = tcp_userver(allocator, host_ip, 
                process_task_cb, opaque);
    } else {
        dbg_str(DBG_WARNNING,"server type error");
        return NULL;
    }

    strcmp(s->type_str,type);

    return s;
}

int server_destroy(server_t *server)
{
    char *type = server->type_str;

    if(!strcmp(type,TCP_ISERVER_TYPE)){
        tcp_iserver_destroy(server);
    } else if (!strcmp(type,TCP_USERVER_TYPE)){
        tcp_userver_destroy(server);
    } else {
        dbg_str(DBG_WARNNING,"server type error");
        return -1;
    }
}
static int test_process_task_callback(void *task)
{
#define MAXLINE 10240
    int nread;
    char buf[MAXLINE];
    int fd = ((server_task_t *)task)->fd;
    nread = read(fd, buf, MAXLINE);//读取客户端socket流

	dbg_str(DBG_VIP,"task start,conn_fd=%d",fd);
    if (nread < 0) {
        dbg_str(DBG_ERROR,"fd read err,client close the connection");
		close(fd);//modify for test
        return;
    } 
    write(fd, buf, nread);//响应客户端  
	/*
	 *close(fd);
	 */
	dbg_str(NET_DETAIL,"task done");
#undef MAXLINE
}
int test_server_of_unix()
{
    allocator_t *allocator = allocator_get_default_alloc();

    dbg_str(DBG_DETAIL,"test_server_of_unix_udp");

    server(allocator,
            TCP_USERVER_TYPE,
            "test_server_un_path", 
            NULL,
            test_process_task_callback,
            NULL);
	return;
}

int test_server_of_inet()
{
    allocator_t *allocator = allocator_get_default_alloc();

    dbg_str(DBG_DETAIL,"test_server_of_unix_udp");

    server(allocator,
            TCP_ISERVER_TYPE,
            "127.0.0.1", 
            "6888",
            test_process_task_callback,
            NULL);
	return;
}

