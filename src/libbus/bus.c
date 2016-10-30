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
int bus_push_args_to_msgblob(struct blob_buf *blob_buf,struct bus_method *method)
{
	void *tbl_arg;
	int i;

	for(i = 0; i < method->n_policy; i++) {
		msgblob_add_u32(blob_buf, method->policy[i].name, method->policy[i].type);
	}
}

int bus_push_method_to_msgblob(struct blob_buf *blob_buf,struct bus_object *obj)
{
	void *tbl_method;
	int i;

	for (i = 0; i < obj->n_methods; i++){
		tbl_method = msgblob_open_table(blob_buf, obj->methods[i].name);
		bus_push_args_to_msgblob(blob_buf,&(obj->methods[i]));
		msgblob_close_table(blob_buf, tbl_method);
	}

	return 0;
}
int bus_add_object(bus_t *bus,struct bus_object *obj)
{
	bus_reqhdr_t hdr;
	static struct blob_buf blob_buf;
	void *tbl_methods;
#define BUS_ADD_OBJECT_MAX_BUFFER_LEN 1024
	uint8_t buffer[BUS_ADD_OBJECT_MAX_BUFFER_LEN];
#undef BUS_ADD_OBJECT_MAX_BUFFER_LEN 
	uint32_t buffer_len;

	memset(&hdr,0,sizeof(hdr));

	hdr.type = BUS_REQ_ADD_OBJECT;

	msgblob_buf_init(&blob_buf);

	msgblob_add_string(&blob_buf, "test", "Hello, world!");
	msgblob_add_u32(&blob_buf, "id", 1);

	tbl_methods = msgblob_open_table(&blob_buf, "methods");
	bus_push_method_to_msgblob(&blob_buf, obj);
	msgblob_close_table(&blob_buf, tbl_methods);

	memcpy(buffer,&hdr, sizeof(hdr));
	buffer_len = sizeof(hdr);
	memcpy(buffer + buffer_len,blob_data(blob_buf.head),blob_len(blob_buf.head));
	buffer_len += blob_len(blob_buf.head);

	dbg_buf(DBG_DETAIL,"bus send:",buffer,buffer_len);
	bus_send(bus, buffer, buffer_len);

	return 0;
}


