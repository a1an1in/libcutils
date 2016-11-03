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
	[BUS_INVOKE_SRC_FD] = { .name = "source_fd",       .type = BLOB_TYPE_INT32 },
	[BUS_INVOKE_DST_FD] = { .name = "destination_fd",  .type = BLOB_TYPE_INT32 },
	[BUS_INVOKE_METHOD] = { .name = "invoke_method",   .type = BLOB_TYPE_STRING },
	[BUS_INVOKE_ARGS]   = { .name = "invoke_args",     .type = BLOB_TYPE_TABLE },
};

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

    bus->blob = blob_create(bus->allocator);
    if(bus->blob == NULL) {
        client_destroy(bus->client);
        dbg_str(DBG_WARNNING,"blob_create");
        return -1;
    }
    blob_init(bus->blob);

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

int bus_add_object(bus_t *bus,struct bus_object *obj)
{
	bus_reqhdr_t hdr;
    blob_t *blob = bus->blob;
#define BUS_ADD_OBJECT_MAX_BUFFER_LEN 1024
	uint8_t buffer[BUS_ADD_OBJECT_MAX_BUFFER_LEN];
#undef BUS_ADD_OBJECT_MAX_BUFFER_LEN 
	uint32_t buffer_len;

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

int bus_invoke(bus_t *bus,char *key, char *method,int argc, char **args)
{
	bus_reqhdr_t hdr;
    blob_t *blob = bus->blob;
#define BUS_ADD_OBJECT_MAX_BUFFER_LEN 1024
	uint8_t buffer[BUS_ADD_OBJECT_MAX_BUFFER_LEN];
#undef BUS_ADD_OBJECT_MAX_BUFFER_LEN 
	uint32_t buffer_len;
    int i;

	memset(&hdr,0,sizeof(hdr));

	hdr.type = BUS_REQ_INVOKE;

    blob_add_table_start(blob,(char *)"invoke"); {
        blob_add_string(blob, (char *)"invoke_key", key);
        blob_add_string(blob, (char *)"invoke_method", method);
        blob_add_u8(blob, (char *)"argc", argc);
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

	dbg_buf(DBG_DETAIL,"bus send:",buffer,buffer_len);
	bus_send(bus, buffer, buffer_len);

	return 0;
}

int bus_handle_add_object_reply(bus_t *bus,  blob_attr_t **attr)
{
	dbg_str(DBG_DETAIL,"bus_handle_add_object_reply");
}

int bus_handle_lookup_object_reply(bus_t *bus,  blob_attr_t **attr)
{
    struct bus_object *obj;
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
        blob_attr_t *attrib,*head;
        uint32_t len;

        head = (blob_attr_t *)blob_get_data(attr[BUS_METHORDS]);
        len  = blob_get_data_len(attr[BUS_METHORDS]);

        blob_for_each_attr(attrib, head, len) {
            dbg_str(DBG_DETAIL,"method name:%s",blob_get_name(attrib));
        }
    }
}

int bus_handle_invoke_reply(bus_t *bus,  blob_attr_t **attr)
{
	dbg_str(DBG_DETAIL,"bus_handle_invoke_reply");
}


int bus_handle_busd_forward_invoke(bus_t *bus,  blob_attr_t **attr)
{
	dbg_str(DBG_VIP,"bus_handle_busd_forward_invoke");
}

static bus_cmd_callback handlers[__BUS_REQ_LAST] = {
	[BUSD_REPLY_ADD_OBJECT] = bus_handle_add_object_reply,
	[BUSD_REPLY_LOOKUP]     = bus_handle_lookup_object_reply,
	[BUSD_REPLY_INVOKE]     = bus_handle_invoke_reply,
	[BUSD_FORWARD_INVOKE]   = bus_handle_busd_forward_invoke,
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
    bus = bus_create(allocator);

    bus_init(bus,//bus_t *bus,
             server_host,//char *server_host,
             server_srv,//char *server_srv,
             bus_process_receiving_data_callback);

    return bus;
}
