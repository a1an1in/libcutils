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
#include <libbus/bus.h>
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
	dbg_str(DBG_DETAIL,"busd:%p",busd);

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

int busd_get_object_method(struct busd_object_method * method,
						   allocator_t *allocator,
						   struct blob_attr *attr)
{
	struct busd_object_method_arg_s arg;
	struct blob_attr *attrib,*head;
	int len;
	struct msgblob_hdr *hdr = (struct msgblob_hdr *)blob_data(attr);

	dbg_str(DBG_DETAIL,"method name:%s",hdr->name);
	method->name = (char *)allocator_mem_alloc(allocator,strlen((char *)hdr->name));
	strncpy(method->name,(char *)hdr->name,strlen((char *)hdr->name));

	method->args = llist_create(allocator,0);
	llist_init(method->args,sizeof(struct busd_object_method_arg_s));

	head = (struct blob_attr *)msgblob_data(attr);
	len  = msgblob_data_len(attr);

	__blob_for_each_attr(attrib, head, len) {
		hdr = (struct msgblob_hdr *)blob_data(attrib);
		dbg_str(DBG_DETAIL,"arg name:%s",hdr->name);
		arg.name = (char *)allocator_mem_alloc(allocator,strlen((char *)hdr->name));
		strncpy(arg.name, (char *)hdr->name, strlen((char *)hdr->name));
		llist_push_back(method->args,&arg);
	}

}

int busd_dump_object(struct busd_object *obj);

int busd_create_bus_object(busd_t *busd,char *name, struct blob_attr *attr)
{
	struct busd_object *obj;
	struct blob_attr *attrib,*head;
	int len;
	struct busd_object_method method; 
	allocator_t *allocator = busd->allocator;

	obj = (struct busd_object *)allocator_mem_alloc(allocator,
													sizeof(struct busd_object));
	if(obj == NULL) {
		dbg_str(DBG_ERROR,"allocator_mem_alloc");
		return -1;
	}
	obj->name = (char *)allocator_mem_alloc(allocator,strlen(name));
	obj->methods = vector_create(allocator, 0);

	vector_init(obj->methods,sizeof(struct busd_object_method),2);

	head = (struct blob_attr *)msgblob_data(attr);
	len  = msgblob_data_len(attr);

	__blob_for_each_attr(attrib, head, len) {
		busd_get_object_method(&method,allocator,attrib);
		vector_push_back(obj->methods,&method);
	}

	busd_dump_object(obj);

	return 0;
}
void busd_dump_object_method_args(list_t *list)
{
	struct busd_object_method_arg_s *arg = (struct busd_object_method_arg_s *)
										   list->data;

	printf("args name:%s\n",arg->name);
}

void busd_dump_object_method(vector_pos_t *pos)
{
	struct busd_object_method *method = (struct busd_object_method *)
										vector_pos_get_pointer(pos);
	printf("method name:%s\n",method->name);

	llist_for_each(method->args,busd_dump_object_method_args);
}
int busd_dump_object(struct busd_object *obj)
{
	printf("obj_name:%s\n",obj->name);

	vector_for_each(obj->methods,(void (*)(void *))(busd_dump_object_method));

}
int busd_handle_add_object(busd_t *busd,  struct blob_attr **attr)
{
	dbg_str(DBG_DETAIL,"ubusd_handle_add_object");

	if (attr[UBUSD_ID]){
		dbg_str(DBG_DETAIL,"rcv id:%d",msgblob_get_u32(attr[UBUSD_ID]));
	}

	if (attr[UBUSD_TEST]) {
		dbg_str(DBG_DETAIL,"rcv test:%s",msgblob_get_string(attr[UBUSD_TEST]));
	}
	if (attr[UBUSD_METHORDS]) {
		busd_create_bus_object(busd,"object name", attr[UBUSD_METHORDS]);
	}

	return 0;
}


static busd_cmd_callback handlers[__BUS_REQ_LAST] = {
	[BUS_REQ_ADD_OBJECT] = busd_handle_add_object,
};

static const struct msgblob_policy ubusd_policy[] = {
	[UBUSD_ID] = { .name = "id", .type = MSGBLOB_TYPE_INT32 },
	[UBUSD_TEST] = { .name = "test", .type = MSGBLOB_TYPE_STRING },
	[UBUSD_METHORDS] = { .name = "methods", .type = MSGBLOB_TYPE_TABLE },
};


static int busd_process_receiving_data_callback(void *task)
{
	server_data_task_t *t = (server_data_task_t *)task;;
	bus_reqhdr_t *hdr;
	struct blob_attr *blob_attr;
	struct blob_attr *tb[__UBUSD_MAX];
	busd_cmd_callback cb = NULL;
	server_t *server = t->server;
	busd_t *busd = (busd_t *)server->opaque;

	hdr = (bus_reqhdr_t *)t->buffer;
	blob_attr = (struct blob_attr *)(t->buffer + sizeof(bus_reqhdr_t));

	if(hdr->type > __BUS_REQ_LAST) {
		dbg_str(DBG_WARNNING,"busd receive err proto type");
		return -1;
	} 

	cb = handlers[hdr->type];

	msgblob_parse(ubusd_policy,
				  ARRAY_SIZE(ubusd_policy),
				  tb,
				  blob_attr,
				  t->buffer_len - sizeof(bus_reqhdr_t));

	cb(busd,tb);

	/*
     *write(t->fd, t->buffer,t->buffer_len);//响应客户端  
	 */

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

