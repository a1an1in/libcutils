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

    return 0;
}

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
						   struct blob_attr_s *attr)
{
	struct busd_object_method_arg_s arg;
	blob_attr_t *pos,*head;
	uint32_t len;

	dbg_str(DBG_DETAIL,"method name:%s",blob_get_name(attr));
	method->name = (char *)allocator_mem_alloc(allocator,strlen(blob_get_name(attr)));
	strncpy(method->name,(char *)blob_get_name(attr),strlen(blob_get_name(attr)));

    method->args = llist_create(allocator,0);
    llist_init(method->args,sizeof(struct busd_object_method_arg_s));

    head = (blob_attr_t *)blob_get_data(attr);
    len  = blob_get_data_len(attr);

    dbg_buf(DBG_DETAIL,"arg:",head,len);
    blob_for_each_attr(pos, head, len) {
        dbg_str(DBG_DETAIL,"arg name:%s",blob_get_name(pos));
        arg.name = (char *)allocator_mem_alloc(allocator,strlen(blob_get_name(pos)));
        strncpy(arg.name, (char *)blob_get_name(pos), strlen(blob_get_name(pos)));
        llist_push_back(method->args,&arg);
    }

    return 0;
}

struct busd_object *
busd_create_bus_object(busd_t *busd,char *name, struct blob_attr_s *attr)
{
	struct busd_object *obj;
	struct blob_attr_s *attrib,*head;
	uint32_t len;
	struct busd_object_method method; 
	allocator_t *allocator = busd->allocator;

	obj = (struct busd_object *)allocator_mem_alloc(allocator,
													sizeof(struct busd_object));
	if(obj == NULL) {
		dbg_str(DBG_ERROR,"allocator_mem_alloc");
		return NULL;
	}

	obj->name = (char *)allocator_mem_alloc(allocator,strlen(name));
    strncpy(obj->name,name,strlen(name));
	obj->methods = vector_create(allocator, 0);

	vector_init(obj->methods,sizeof(struct busd_object_method),2);

	head = (struct blob_attr_s *)blob_get_data(attr);
	len  = blob_get_data_len(attr);

	blob_for_each_attr(attrib, head, len) {
		busd_get_object_method(&method,allocator,attrib);
		vector_push_back(obj->methods,&method);
	}

    /*
	 *busd_dump_object(obj);
     */

	return obj;
}

int busd_handle_add_object(busd_t *busd,  struct blob_attr_s **attr)
{
	dbg_str(DBG_DETAIL,"ubusd_handle_add_object");

	if (attr[BUSD_ID]){
		dbg_str(DBG_DETAIL,"object id:%d",blob_get_u32(attr[BUSD_ID]));
	}
	if (attr[BUSD_OBJNAME]) {
		dbg_str(DBG_DETAIL,"object name:%s",blob_get_string(attr[BUSD_OBJNAME]));
	}
    if (attr[BUSD_METHORDS]) {
        struct busd_object *obj;

        obj = busd_create_bus_object(busd,blob_get_string(attr[BUSD_OBJNAME]), attr[BUSD_METHORDS]);
        if(obj != NULL){
        }
	}

	return 0;
}

static busd_cmd_callback handlers[__BUS_REQ_LAST] = {
	[BUS_REQ_ADD_OBJECT] = busd_handle_add_object,
};

static const struct blob_policy_s busd_policy[] = {
	[BUSD_ID]       = { .name = "id", .type = BLOB_TYPE_INT32 },
	[BUSD_OBJNAME]  = { .name = "object_name", .type = BLOB_TYPE_STRING },
	[BUSD_METHORDS] = { .name = "methods", .type = BLOB_TYPE_TABLE },
};

static int busd_process_receiving_data_callback(void *task)
{
	server_data_task_t *t = (server_data_task_t *)task;;
	bus_reqhdr_t *hdr;
	struct blob_attr_s *blob_attr;
	struct blob_attr_s *tb[__BUSD_MAX];
	busd_cmd_callback cb = NULL;
	server_t *server = t->server;
	busd_t *busd = (busd_t *)server->opaque;

	hdr = (bus_reqhdr_t *)t->buffer;
	blob_attr = (struct blob_attr_s *)(t->buffer + sizeof(bus_reqhdr_t));

	if(hdr->type > __BUS_REQ_LAST) {
		dbg_str(DBG_WARNNING,"busd receive err proto type");
		return -1;
	} 

	cb = handlers[hdr->type];

    /*
     *printf_buffer(blob_attr,blob_get_data_len(blob_attr));
     */

    int len = blob_get_data_len(blob_attr);
    blob_attr = blob_get_data(blob_attr);
    dbg_buf(DBG_DETAIL,"rcv oject:",blob_attr,len);
    blob_parse(busd_policy,
               ARRAY_SIZE(busd_policy),
               tb,
               blob_attr,
               len);

    cb(busd,tb);

    /*
     *printf_buffer(t->buffer,t->buffer_len);
     */
	/*
     *write(t->fd, t->buffer,t->buffer_len);//响应客户端  
	 */

    return 0;
}

void test_bus_daemon()
{
    allocator_t *allocator = allocator_get_default_alloc();
    busd_t *busd;
    char *server_host = (char *)"bus_server_path";
    char *server_srv = NULL;
    
    dbg_str(DBG_DETAIL,"test_busd_daemon");
    busd = busd_create(allocator);

    busd_init(busd,//busd_t *busd,
              server_host,//char *server_host,
              server_srv,//char *server_srv,
              busd_process_receiving_data_callback);
}

