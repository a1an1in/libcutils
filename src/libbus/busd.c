/**
 * @file busd.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 
 * @date 2016-10-28
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
#include <unistd.h>
#include <libdbg/debug.h>
#include <libbus/busd.h>
#include <libipc/net/server.h>

busd_t * busd_create(allocator_t *allocator)
{
    busd_t *d;

    d = (busd_t *)allocator_mem_alloc(allocator,sizeof(busd_t));
    if( d == NULL) {
        dbg_str(DBG_DETAIL,"allocator_mem_alloc");
        return NULL;
    }

    memset(d,0, sizeof(busd_t));

    d->allocator = allocator;

    return d;
}

int busd_init(busd_t *busd,
              char *server_host,
              char *server_srv,
              int (*process_server_task_cb)(void *task))
{
    if(busd->server_sk_type == NULL){
        busd->server_sk_type = (char *)(&(SERVER_TYPE_TCP_UNIX));
    }
    busd->server_host = server_host;
    busd->server_srv  = server_srv;

    busd->server= server(busd->allocator,
                         busd->server_sk_type,
                         busd->server_host, 
                         busd->server_srv,
                         process_server_task_cb,
                         busd);
    if(busd->server == NULL) {
        dbg_str(DBG_ERROR,"busd_init,create server");
        return -1;
    }

    return 1;
}

static int busd_process_receiving_data_callback(void *task)
{
	server_data_task_t *t = (server_data_task_t *)task;;

    write(t->fd, t->buffer,t->buffer_len);//响应客户端  

    return 0;
}

void test_bus_daemon()
{
    allocator_t *allocator = allocator_get_default_alloc();
    busd_t *busd;
    char *server_host = "bus_server_path";
    char *server_srv = NULL;
    
    dbg_str(DBG_DETAIL,"test_busd_daemon");
    busd = busd_create(allocator);

    busd_init(busd,//busd_t *busd,
              server_host,//char *server_host,
              server_srv,//char *server_srv,
              busd_process_receiving_data_callback);
}

