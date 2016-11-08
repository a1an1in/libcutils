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
#include <libblob/blob.h>

static const blob_policy_t bus_policy[] = {
	[BUS_ID]            = { .name = "id",              .type = BLOB_TYPE_INT32 },
	[BUS_OBJNAME]       = { .name = "object_name",     .type = BLOB_TYPE_STRING },
	[BUS_METHORDS]      = { .name = "methods",         .type = BLOB_TYPE_TABLE },
	[BUS_STATE]         = { .name = "state",           .type = BLOB_TYPE_INT32 },
	[BUS_OPAQUE]        = { .name = "opaque",          .type = BLOB_TYPE_BUFFER },
	[BUS_INVOKE_SRC_FD] = { .name = "source_fd",       .type = BLOB_TYPE_INT32 },
	[BUS_INVOKE_DST_FD] = { .name = "destination_fd",  .type = BLOB_TYPE_INT32 },
	[BUS_INVOKE_METHOD] = { .name = "invoke_method",   .type = BLOB_TYPE_STRING },
	[BUS_INVOKE_ARGC]   = { .name = "invoke_argc",     .type = BLOB_TYPE_INT32 },
	[BUS_INVOKE_ARGS]   = { .name = "invoke_args",     .type = BLOB_TYPE_TABLE },
};

static void addr_to_buffer(void *addr,uint8_t *buffer)
{
	unsigned long data = (unsigned long)addr;
	int i;

	for(i = 0; i < sizeof(int *); i++){
		buffer[i] = data >> 8 * (sizeof(int *) - 1 - i);
	}
}

static void *buffer_to_addr(uint8_t *buffer)
{
	void *ret = NULL;
	unsigned long d = 0, t = 0;
	int i;

	for ( i = 0; i < sizeof(int *); i++){
		t = buffer[i];
		d |= t << 8 * (sizeof(int *) - 1 - i); 
	}

	ret = (void *)d;

	return ret;
}

bus_t * bus_alloc(allocator_t *allocator)
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

    bus->blob = blob_create(bus->allocator);
    if(bus->blob == NULL) {
        client_destroy(bus->client);
        dbg_str(DBG_WARNNING,"blob_create");
        return -1;
    }
    blob_init(bus->blob);

    /*create object hash map*/
    if(bus->key_size == 0) {
        bus->key_size = 10;
    }
    if(bus->bucket_size == 0) {
        bus->bucket_size = 20;
    }

	bus->obj_hmap = hash_map_create(bus->allocator,0);
    if(bus->obj_hmap == NULL) {
        //.........
        return -1;
    }

	hash_map_init(bus->obj_hmap,
			      bus->key_size,//uint32_t key_size,
			      sizeof(bus_object_t)+ bus->key_size,
			      bus->bucket_size,
			      NULL,
			      NULL);

    /*create req hash map*/
    if(bus->req_key_size == 0) {
        bus->req_key_size = 10;
    }
    if(bus->req_bucket_size == 0) {
        bus->req_bucket_size = 20;
    }

	bus->req_hmap = hash_map_create(bus->allocator,0);
    if(bus->req_hmap == NULL) {
        //.........
        return -1;
    }

	hash_map_init(bus->req_hmap,
			      bus->req_key_size,//uint32_t req_key_size,
			      sizeof(bus_object_t)+ bus->req_key_size,
			      bus->req_bucket_size,
			      NULL,
			      NULL);

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

int bus_push_args_to_blob(blob_t *blob,struct bus_method *method)
{
	int i;
    /*
	 *blob_attr_t *t1,*t2;
     */

    /*
     *t1 = blob->tail;
     */
	for(i = 0; i < method->n_policy; i++) {
		blob_add_u32(blob, method->policy[i].name, method->policy[i].type);
	}
    /*
     *t2 = blob->tail;
     *uint16_t len = (uint8_t *)t2 - (uint8_t *)t1;
     *dbg_buf(DBG_DETAIL,"***********push args:",(void *)t1, len);
     */
    return 0;
}

int bus_push_methods_to_blob(blob_t *blob,struct bus_object *obj)
{
	int i;

    dbg_str(DBG_DETAIL,"bus_push_method_to_blob,n_methods=%d",obj->n_methods);

	for (i = 0; i < obj->n_methods; i++){
        dbg_str(DBG_DETAIL,"push method:%s",obj->methods[i].name);
        blob_add_table_start(blob, obj->methods[i].name);
        bus_push_args_to_blob(blob,&(obj->methods[i]));
        blob_add_table_end(blob);
        dbg_str(DBG_DETAIL,"push method end");
	}

    dbg_str(DBG_DETAIL,"bus_push_methods_to_blob end");

	return 0;
}
int __bus_add_obj(bus_t *bus,struct bus_object *obj)
{
    uint8_t addr_buffer[8];

    addr_to_buffer(obj,addr_buffer);
    hash_map_insert(bus->obj_hmap,obj->name,addr_buffer);

    return 0;
}
int bus_add_object(bus_t *bus,struct bus_object *obj)
{
	bus_reqhdr_t hdr;
    blob_t *blob = bus->blob;
#define BUS_ADD_OBJECT_MAX_BUFFER_LEN 1024
	uint8_t buffer[BUS_ADD_OBJECT_MAX_BUFFER_LEN];
#undef BUS_ADD_OBJECT_MAX_BUFFER_LEN 
	uint32_t buffer_len;

    dbg_str(DBG_DETAIL,"bus_add_object,obj addr:%p",obj);
	memset(&hdr,0,sizeof(hdr));

	hdr.type = BUS_REQ_ADD_OBJECT;

    blob_add_table_start(blob,(char *)"object"); {
        blob_add_string(blob, (char *)"object_name", obj->name);
        blob_add_u32(blob, (char *)"id", 1);
        blob_add_table_start(blob, (char *)"methods"); {
            bus_push_methods_to_blob(blob, obj);
        }
        blob_add_table_end(blob);
    }
    blob_add_table_end(blob);

    /*
     *dbg_str(DBG_DETAIL,"run at here");
     */
	memcpy(buffer,&hdr, sizeof(hdr));
	buffer_len = sizeof(hdr);
    /*
     *dbg_buf(DBG_DETAIL,"object:",blob->head,blob_get_len(blob->head));
     */
	memcpy(buffer + buffer_len,(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
	buffer_len += blob_get_len((blob_attr_t *)blob->head);

	dbg_buf(DBG_DETAIL,"bus send:",buffer,buffer_len);
	bus_send(bus, buffer, buffer_len);

    __bus_add_obj(bus,obj);

	return 0;
}

int bus_handle_add_object_reply(bus_t *bus,  blob_attr_t **attr)
{
    int state;

	dbg_str(DBG_DETAIL,"bus_handle_add_object_reply");

	if (attr[BUS_STATE]){
        state = blob_get_u32(attr[BUS_STATE]);
		dbg_str(DBG_DETAIL,"state=%d",state);
	}

	if (attr[BUS_OBJNAME]) {
		dbg_str(DBG_DETAIL,"object name:%s",blob_get_string(attr[BUS_OBJNAME]));
	}

    if( state == 1) {
        dbg_str(DBG_SUC,"add obj success");
    } else {
        dbg_str(DBG_ERROR,"add obj failed");
        //..... del the obj
    }

    return 0;
}

int bus_lookup(bus_t *bus, char *key)
{
	bus_reqhdr_t hdr;
    blob_t *blob = bus->blob;
#define BUS_ADD_OBJECT_MAX_BUFFER_LEN 1024
	uint8_t buffer[BUS_ADD_OBJECT_MAX_BUFFER_LEN];
#undef BUS_ADD_OBJECT_MAX_BUFFER_LEN 
	uint32_t buffer_len;

	memset(&hdr,0,sizeof(hdr));

	hdr.type = BUS_REQ_LOOKUP;

    blob_add_table_start(blob,(char *)"lookup"); {
        blob_add_string(blob, (char *)"object_name", key);
    }
    blob_add_table_end(blob);

    /*
     *dbg_str(DBG_DETAIL,"run at here");
     */
	memcpy(buffer,&hdr, sizeof(hdr));
	buffer_len = sizeof(hdr);
    /*
     *dbg_buf(DBG_DETAIL,"object:",blob->head,blob_get_len(blob->head));
     */
	memcpy(buffer + buffer_len,(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
	buffer_len += blob_get_len((blob_attr_t *)blob->head);

	dbg_buf(DBG_DETAIL,"bus send:",buffer,buffer_len);
	bus_send(bus, buffer, buffer_len);

	return 0;
}

int bus_handle_lookup_object_reply(bus_t *bus,  blob_attr_t **attr)
{
    struct bus_object *obj;
    blob_attr_t *attrib,*head;
    uint32_t len;
    int ret;

	dbg_str(DBG_DETAIL,"bus_handle_lookup_object_reply");

	if (attr[BUS_ID]){
		dbg_str(DBG_DETAIL,"object id:%d",blob_get_u32(attr[BUS_ID]));
	}
	if (attr[BUS_OBJNAME]) {
		dbg_str(DBG_DETAIL,"object name:%s",blob_get_string(attr[BUS_OBJNAME]));
	}
    if (attr[BUS_METHORDS]) {
        dbg_str(DBG_DETAIL,"object methods");

        head = (blob_attr_t *)blob_get_data(attr[BUS_METHORDS]);
        len  = blob_get_data_len(attr[BUS_METHORDS]);

        blob_for_each_attr(attrib, head, len) {
            dbg_str(DBG_DETAIL,"method name:%s",blob_get_name(attrib));
        }
    }
}

int bus_invoke(bus_t *bus,char *key, char *method,int argc, char **args)
{
	bus_reqhdr_t hdr;
    blob_t *blob = bus->blob;
#define BUS_ADD_OBJECT_MAX_BUFFER_LEN 1024
	uint8_t buffer[BUS_ADD_OBJECT_MAX_BUFFER_LEN];
#undef BUS_ADD_OBJECT_MAX_BUFFER_LEN 
	uint32_t buffer_len;
    int i;

    /*compose req proto*/
	memset(&hdr,0,sizeof(hdr));

	hdr.type = BUS_REQ_INVOKE;

    blob_add_table_start(blob,(char *)"invoke"); {
        blob_add_string(blob, (char *)"invoke_key", key);
        blob_add_string(blob, (char *)"invoke_method", method);
        blob_add_u8(blob, (char *)"invoke_argc", argc);
        blob_add_table_start(blob, (char *)"invoke_args"); {
            for(i = 0; i < argc; i++) {
                blob_add_string(blob, (char *)"arg", args[i]);
            }
        }
        blob_add_table_end(blob);
    }
    blob_add_table_end(blob);

	memcpy(buffer,&hdr, sizeof(hdr));
	buffer_len = sizeof(hdr);
	memcpy(buffer + buffer_len,(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
	buffer_len += blob_get_len((blob_attr_t *)blob->head);

    /*send req proto*/
	dbg_buf(DBG_DETAIL,"bus send:",buffer,buffer_len);
	bus_send(bus, buffer, buffer_len);

	return 0;
}
int bus_invoke_async(bus_t *bus,char *key, char *method,int argc, char **args)
{
/*
 *    bus_req_t req;
 *
 *    bus_invoke(bus,key, method,argc, args);
 *
 *    req.method = method;
 *    req.state = -1;
 *
 *    make_pair(bus->pair,method,&req);
 *    hash_map_insert_data(bus->obj_hmap,bus->pair->data);
 */

    return 0;
}

int bus_invoke_sync(bus_t *bus,char *key, char *method,int argc, char **args,char *out_buf,char *out_len)
{
    bus_req_t req,*req_back;
    hash_map_pos_t out;
    int ret;
    int count = 0;
    int state = 0;

#define MAX_BUFFER_LEN 2048
    char buffer[MAX_BUFFER_LEN];
#undef MAX_BUFFER_LEN

    req.method            = method;
    req.state             = 0xffff;
    req.opaque            = NULL;
    req.opaque_len        = 0;
    req.opaque_buffer_len = 0;

    ret = hash_map_insert_wb(bus->req_hmap,method,&req,&out);
    if (ret < 0) {
        dbg_str(DBG_WARNNING,"bus_invoke_sync");
        return ret;
    }

    req_back                    = (bus_req_t *)hash_map_pos_get_pointer(&out);
    req_back->opaque            = buffer;
    req_back->opaque_buffer_len = sizeof(buffer);

    bus_invoke(bus,key, method,argc, args);

    while(req_back->state == 0xffff) {
        sleep(1);
        count++;
        if(count > 5) {
            req_back->state = -99;
            break;
        }
    }

    dbg_str(DBG_DETAIL,"bus_invoke_sync,rev return state =%d",req_back->state);
    dbg_buf(DBG_DETAIL,"opaque:",req_back->opaque,req_back->opaque_len);

    memcpy(out_buf,req_back->opaque,req_back->opaque_len);
    *out_len = req_back->opaque_len;

    state = req_back->state;

    hash_map_delete(bus->req_hmap, &out);

    return state;
}

int bus_handle_invoke_reply(bus_t *bus,  blob_attr_t **attr)
{
    char *obj_name, *method_name;
    hash_map_pos_t pos;
    bus_req_t *req;
    int state;
    int ret;
    char *buffer = NULL;
    int buffer_len = 0;

	dbg_str(DBG_DETAIL,"bus_handle_invoke_reply");

	if (attr[BUS_STATE]){
        state = blob_get_u32(attr[BUS_STATE]);
		dbg_str(DBG_DETAIL,"state:%d",state);
	}
	if (attr[BUS_OBJNAME]) {
		obj_name = blob_get_string(attr[BUS_OBJNAME]);
		dbg_str(DBG_DETAIL,"object name:%s",obj_name);
	}
    if (attr[BUS_INVOKE_METHOD]) {
		method_name = blob_get_string(attr[BUS_INVOKE_METHOD]);
		dbg_str(DBG_DETAIL,"method name:%s",method_name);
    }
	if (attr[BUS_OPAQUE]){
        buffer_len = blob_get_buffer(attr[BUS_OPAQUE],&buffer);
        dbg_buf(DBG_DETAIL,"bus_handle_invoke_reply,buffer:",buffer,buffer_len);
	}

    if(method_name != NULL) {
        ret = hash_map_search(bus->req_hmap, method_name ,&pos);
        if(ret > 0) {
            req = (bus_req_t *)hash_map_pos_get_pointer(&pos);
            req->state = state;
            if(req->opaque_buffer_len < buffer_len) {
                dbg_str(DBG_WARNNING,"opaque buffer is too small , please check");
            }
            req->opaque_len = buffer_len;
            memcpy(req->opaque,buffer,buffer_len);
            dbg_str(DBG_DETAIL,"method_name:%s,state:%d",req->method,req->state);
        }
    }

    return 0;
}

bus_handler_t 
bus_get_method_handler(bus_object_t *obj,char *method)
{
    int i;

    for(i = 0; i < obj->n_methods; i++) {
        if(!strncmp(obj->methods[i].name,method,sizeof(obj->methods[i].name))) {
            return obj->methods[i].handler;
        }
    }

    return NULL;
}

blob_policy_t * 
bus_get_policy(bus_object_t *obj,char *method)
{
    int i;

    for(i = 0; i < obj->n_methods; i++) {
        if(!strncmp(obj->methods[i].name,method,sizeof(obj->methods[i].name))) {
            return obj->methods[i].policy;
        }
    }

    return NULL;
}

int bus_reply_forward_invoke(bus_t *bus, char *obj_name,char *method_name, int ret, char *buf, int buf_len,int src_fd)
{
#define BUS_ADD_OBJECT_MAX_BUFFER_LEN 2048
	bus_reqhdr_t hdr;
    blob_t *blob;
	uint8_t buffer[BUS_ADD_OBJECT_MAX_BUFFER_LEN];
	uint32_t buffer_len,tmp_len;
	allocator_t *allocator = bus->allocator;

	dbg_str(DBG_DETAIL,"bus_reply_forward_invoke,ret = %d", ret);
	memset(&hdr,0,sizeof(hdr));

	hdr.type = BUS_REPLY_FORWARD_INVOKE;

    blob = blob_create(allocator);
    if(blob == NULL) {
        dbg_str(DBG_WARNNING,"blob_create");
        return -1;
    }
    blob_init(blob);
    blob_add_table_start(blob,(char *)"reply_forward_invoke"); {
        blob_add_string(blob, (char *)"object_name", obj_name);
        blob_add_string(blob, (char *)"invoke_method", method_name);
        blob_add_u32(blob, (char *)"state", ret);
        blob_add_buffer(blob, (char *)"opaque", buf, buf_len);
        blob_add_u32(blob, (char *)"source_fd", src_fd);
    }
    blob_add_table_end(blob);

	memcpy(buffer,&hdr, sizeof(hdr));
	buffer_len = sizeof(hdr);

    tmp_len = buffer_len + blob_get_len((blob_attr_t *)blob->head);

    if(tmp_len > BUS_ADD_OBJECT_MAX_BUFFER_LEN) {
        dbg_str(DBG_WARNNING,"buffer is too small,please check");
        return -1;
    }

	memcpy(buffer + buffer_len,(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
	buffer_len += blob_get_len((blob_attr_t *)blob->head);

	dbg_buf(DBG_DETAIL,"bus send:",buffer,buffer_len);

	bus_send(bus, buffer, buffer_len);

	return 0;
#undef BUS_ADD_OBJECT_MAX_BUFFER_LEN 
}
int bus_handle_forward_invoke(bus_t *bus,  blob_attr_t **attr)
{
    bus_object_t *obj = NULL;
    blob_attr_t *args = NULL;
    int argc = 0;
    char *method_name = NULL;
    char *obj_name;
    int src_fd = -1;
    hash_map_pos_t pos;
    bus_handler_t method;
    uint8_t *p;
    blob_policy_t *policy;
    struct blob_attr_s *tb[10];
#define MAX_BUFFER_LEN 2048
    char buffer[MAX_BUFFER_LEN];
    int ret, buffer_len = 9;

	dbg_str(DBG_VIP,"bus_handle_forward_invoke");

	if (attr[BUS_INVOKE_SRC_FD]) {
        src_fd = blob_get_u32(attr[BUS_INVOKE_SRC_FD]);
		dbg_str(DBG_DETAIL,"invoke src fd:%d",src_fd);
	}
	if (attr[BUS_INVOKE_METHOD]) {
        method_name = blob_get_string(attr[BUS_INVOKE_METHOD]);
		dbg_str(DBG_DETAIL,"invoke method_name:%s",method_name);
	}
	if (attr[BUS_INVOKE_ARGC]) {
        argc = blob_get_u8(attr[BUS_INVOKE_ARGC]);
        dbg_str(DBG_DETAIL,"invoke argc=%d",argc);
	}
	if (attr[BUS_INVOKE_ARGS]) {
        dbg_str(DBG_DETAIL,"invoke args");
        args = attr[BUS_INVOKE_ARGS];
	}
	if (attr[BUS_OBJNAME]) {
        obj_name = blob_get_string(attr[BUS_OBJNAME]);
		dbg_str(DBG_DETAIL,"object name:%s",obj_name);
	}

    if (method_name != NULL) {
        ret = hash_map_search(bus->obj_hmap, obj_name ,&pos);
        if(ret > 0) {
            p = (uint8_t *)hash_map_pos_get_pointer(&pos);
            obj = (bus_object_t *)buffer_to_addr(p);
            dbg_str(DBG_DETAIL,"obj addr:%p",obj);

            method = bus_get_method_handler(obj,method_name);
            policy = bus_get_policy(obj,method_name);

            blob_parse(policy, ARRAY_SIZE(policy), tb, blob_get_data(args), blob_get_data_len(args));
            ret = method(bus,argc,tb,buffer,&buffer_len);
            if(buffer_len > MAX_BUFFER_LEN) {
                dbg_str(DBG_WARNNING,"buffer is too small,please check");
            } 
            bus_reply_forward_invoke(bus,obj_name,method_name, ret, buffer, buffer_len,src_fd);
        }
    }

    return 0;
#undef MAX_BUFFER_LEN 
}

static bus_cmd_callback handlers[__BUS_REQ_LAST] = {
	[BUSD_REPLY_ADD_OBJECT] = bus_handle_add_object_reply,
	[BUSD_REPLY_LOOKUP]     = bus_handle_lookup_object_reply,
	[BUSD_REPLY_INVOKE]     = bus_handle_invoke_reply,
	[BUSD_FORWARD_INVOKE]   = bus_handle_forward_invoke,
};

static int bus_process_receiving_data_callback(client_task_t *task)
{
	bus_reqhdr_t *hdr;
	blob_attr_t *blob_attr;
	blob_attr_t *tb[__BUS_MAX];
	bus_cmd_callback cb = NULL;
	client_t *client = task->client;
	bus_t *bus = (bus_t *)client->opaque;
    int len;

	dbg_str(DBG_DETAIL,"bus_process_receiving_data_callback");
	dbg_buf(DBG_DETAIL,"task buffer:",task->buffer,task->buffer_len);

	hdr = (bus_reqhdr_t *)task->buffer;
	blob_attr = (blob_attr_t *)(task->buffer + sizeof(bus_reqhdr_t));

	if(hdr->type > __BUS_REQ_LAST) {
		dbg_str(DBG_WARNNING,"bus receive err proto type");
		return -1;
	} 

	cb = handlers[hdr->type];

    len = blob_get_data_len(blob_attr);
    blob_attr =(blob_attr_t*) blob_get_data(blob_attr);

    blob_parse(bus_policy,
               ARRAY_SIZE(bus_policy),
               tb,
               blob_attr,
               len);

    cb(bus,tb);

	dbg_str(DBG_DETAIL,"process_rcv end");
    return 0;
}

bus_t * bus_client_create(allocator_t *allocator,
                          char *server_host,
                          char *server_srv)
{
    bus_t *bus;
    
    dbg_str(DBG_DETAIL,"bus_client_create");
    bus = bus_alloc(allocator);

    bus_init(bus,//bus_t *bus,
             server_host,//char *server_host,
             server_srv,//char *server_srv,
             bus_process_receiving_data_callback);

    return bus;
}
