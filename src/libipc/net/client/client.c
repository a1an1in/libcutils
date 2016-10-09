/**
 * @file client.c
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
#include <signal.h>
#include <libipc/net/client.h>
#include <libdbg/debug.h>

client_t *client(allocator_t *allocator,
                 char *id_str,
                 char *serve_no,
                 char *type,
                 int (*process_task_cb)(client_task_t *task),
                 void *opaque)
{
    client_t *c;

    if(!strcpy(type,TCP_ICLIENT_TYPE)){
        c = tcp_iclient(allocator, id_str, serve_no,
                process_task_cb, opaque);
    } else if (!strcpy(type,UDP_ICLIENT_TYPE)){
        c = udp_iclient(allocator, id_str, serve_no,
                process_task_cb, opaque);
    } else if (!strcpy(type,UDP_UCLIENT_TYPE)){
        c = udp_uclient(allocator, id_str, process_task_cb, opaque);
    } else if (!strcpy(type,TCP_UCLIENT_TYPE)){
        c = tcp_uclient(allocator, id_str, process_task_cb, opaque);
    } else {
        dbg_str(DBG_WARNNING,"client error type");
        return NULL;
    }

    return c;
}
int client_send(client_t *client,const void *buf,size_t nbytes,int flags,
        char *type, char *dest_id_str, char *dest_srv_str)
{
    int ret;

    if(!strcpy(type,TCP_ICLIENT_TYPE)){
        ret = tcp_iclient_send(client,buf,nbytes,flags);
    } else if (!strcpy(type,UDP_ICLIENT_TYPE)){
        struct sockaddr_in raddr;
        raddr.sin_family = AF_INET; 
        raddr.sin_port = htons(atoi(dest_srv_str));  
        inet_pton(AF_INET,dest_id_str,&raddr.sin_addr);

        ret = udp_iclient_send(client,buf,nbytes,flags,
                (struct sockaddr *)&raddr,sizeof(raddr));

    } else if (!strcpy(type,UDP_UCLIENT_TYPE)){
        ret = udp_uclient_send(client,buf,nbytes,flags, dest_id_str);
    } else if (!strcpy(type,TCP_UCLIENT_TYPE)){
        ret = tcp_uclient_send(client,buf,nbytes,flags);
    } else {
        dbg_str(DBG_WARNNING,"client error type");
        return -1;
    }

    return ret;
}
int client_destroy(client_t *client)
{
	allocator_mem_free(client->allocator,client);
    //del event.........

	return 0;
}
