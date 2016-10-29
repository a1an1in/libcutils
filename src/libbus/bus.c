/**
 * @file bus.c
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
#include <libbus/bus.h>

bus_t * bus_create(allocator_t *allocator)
{
    bus_t *b;

    b = (bus_t *)allocator_mem_alloc(allocator,sizeof(bus_t));
    if( b == NULL) {
        dbg_str(DBG_DETAIL,"allocator_mem_alloc");
        return NULL;
    }

    memset(b,0, sizeof(bus_t));

    b->allocator = allocator;

    return b;
}

int bus_init(bus_t *bus,
             char *server_host,
             char *server_srv,
             int (*process_client_task_cb)(client_task_t *task))
{
    if(bus->client_sk_type == NULL){
        bus->client_sk_type = (char *)(&(CLIENT_TYPE_TCP_UNIX));
    }
    bus->server_host = server_host;
    bus->server_srv  = server_srv;

    bus->client = client(bus->allocator,
                         bus->client_sk_type,
                         bus->server_host,
                         bus->server_srv,
                         process_client_task_cb,
                         bus);

    return 1;
                         
}
int bus_send(bus_t *bus,
			 void *buf,
			 size_t buf_len)
{
	int ret = 0;
	ret = client_send(bus->client,
				   	  buf,//const void *buf,
					  buf_len,//size_t nbytes,
					  0,//int flags,
					  bus->server_host,//char *dest_id_str, 
				      bus->server_srv);//char *dest_srv_str)
	if(ret < 0){
		dbg_str(DBG_WARNNING,"bus send err");
	}

	return ret;
}


static int bus_process_receiving_data_callback(client_task_t *task)
{
	dbg_str(DBG_DETAIL,"process_task begin,unix client recv");
	/*
	 *user_t *user = task->client;
	 *void *opaque = user->opaque;
	 */
	dbg_buf(DBG_VIP,"task buffer:",task->buffer,task->buffer_len);
	dbg_str(DBG_DETAIL,"%s",task->buffer);
	dbg_str(DBG_DETAIL,"process_task end");

    return 0;
}

void test_bus_client()
{
    allocator_t *allocator = allocator_get_default_alloc();
    bus_t *bus;
    char *server_host = "bus_server_path";
    char *server_srv = NULL;
	char buf[1024] = "hello world!";
	int buf_len = strlen(buf);
    
    dbg_str(DBG_DETAIL,"test_busd_daemon");
    bus = bus_create(allocator);

    bus_init(bus,//busd_t *busd,
             server_host,//char *server_host,
             server_srv,//char *server_srv,
             bus_process_receiving_data_callback);

    dbg_str(DBG_DETAIL,"test_bus_client");
	
	bus_send(bus, buf,buf_len);
}
