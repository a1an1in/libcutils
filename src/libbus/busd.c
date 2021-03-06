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
#include <miscellany/buffer.h>

static const struct blob_policy_s busd_policy[] = {
    [BUSD_ID]             = { .name = "id",              .type = BLOB_TYPE_INT32 },
    [BUSD_OBJNAME]        = { .name = "object_name",     .type = BLOB_TYPE_STRING },
    [BUSD_METHORDS]       = { .name = "methods",         .type = BLOB_TYPE_TABLE },
    [BUSD_INVOKE_KEY]     = { .name = "invoke_key",      .type = BLOB_TYPE_STRING },
    [BUSD_INVOKE_METHORD] = { .name = "invoke_method",   .type = BLOB_TYPE_STRING },
    [BUSD_INVOKE_ARGC]    = { .name = "invoke_argc",     .type = BLOB_TYPE_INT32 },
    [BUSD_INVOKE_ARGS]    = { .name = "invoke_args",     .type = BLOB_TYPE_STRING },
    [BUSD_STATE]          = { .name = "state",           .type = BLOB_TYPE_INT32 },
    [BUSD_OPAQUE]         = { .name = "opaque",          .type = BLOB_TYPE_INT32 },
    [BUSD_INVOKE_SRC_FD]  = { .name = "source_fd",       .type = BLOB_TYPE_INT32 },
};

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

busd_t * busd_alloc(allocator_t *allocator)
{
    busd_t *d;

    d = (busd_t *)allocator_mem_alloc(allocator,sizeof(busd_t));
    if ( d == NULL) {
        dbg_str(BUS_DETAIL,"allocator_mem_alloc");
        return NULL;
    }

    memset(d,0, sizeof(busd_t));

    d->allocator = allocator;

    return d;
}

int busd_set(busd_t *busd,char *attrib_name, char *value, int value_len)
{
    if (!strcmp(attrib_name,"server_sk_type")) {
        busd->server_sk_type = value;
    }else if (!strcmp(attrib_name,"server_host")) {
        busd->server_host = value;
    }else{
        dbg_str(DBG_WARNNING,"not support attrib setting,please check");
    }

    return 0;
}

int busd_init(busd_t *busd,
              char *server_host,
              char *server_srv,
              int (*process_server_task_cb)(void *task))
{
    int ret; 

    if (busd->server_sk_type == NULL) {
        busd->server_sk_type = (char *)(&(SERVER_TYPE_UNIX_TCP));
    }
    busd->server_host = server_host;
    busd->server_srv  = server_srv;
    dbg_str(BUS_DETAIL,"busd:%p",busd);

    busd->server= server(busd->allocator,
                         busd->server_sk_type,
                         busd->server_host, 
                         busd->server_srv,
                         process_server_task_cb,
                         busd);
    if (busd->server == NULL) {
        dbg_str(BUS_ERROR,"busd_init,create server");
        return -1;
    }

    /*create object hash map*/
    if (busd->key_size == 0) {
        busd->key_size = 10;
    }
    if (busd->bucket_size == 0) {
        busd->bucket_size = 20;
    }

    busd->obj_hmap = hash_map_alloc(busd->allocator);
    if (busd->obj_hmap == NULL) {
        server_destroy(busd->server);
        return -1;
    }

    hash_map_init(busd->obj_hmap,
                  busd->key_size,//uint32_t key_size,
                  sizeof(busd_object_t),
                  busd->bucket_size);

    return 1;
}

int busd_get_object_method(struct busd_object_method * method,
                           allocator_t *allocator,
                           blob_attr_t *attr)
{
    struct busd_object_method_arg_s arg;
    blob_attr_t *pos,*head;
    uint32_t len;
    int data_size = sizeof(struct busd_object_method_arg_s);
    int lock_type = 0;

    dbg_str(BUS_DETAIL,"method name:%s",blob_get_name(attr));
    method->name = (char *)allocator_mem_alloc(allocator,strlen(blob_get_name(attr)));
    strncpy(method->name,(char *)blob_get_name(attr),strlen(blob_get_name(attr)));

    method->args = llist_alloc(allocator);
    llist_set(method->args,"lock_type",&lock_type);
    llist_set(method->args,"data_size",&data_size);
    llist_init(method->args);

    head = (blob_attr_t *)blob_get_data(attr);
    len  = blob_get_data_len(attr);

    dbg_buf(BUS_DETAIL,"arg:",(uint8_t *)head,len);

    blob_for_each_attr(pos, head, len) {
        dbg_str(BUS_DETAIL,"arg name:%s",blob_get_name(pos));
        arg.name = (char *)allocator_mem_alloc(allocator,strlen(blob_get_name(pos)));
        strncpy(arg.name, (char *)blob_get_name(pos), strlen(blob_get_name(pos)));
        llist_push_back(method->args,&arg);
    }

    return 0;
}

struct busd_object *
busd_create_bus_object(busd_t *busd,char *name, blob_attr_t *attr,int fd)
{
    struct busd_object *obj;
    blob_attr_t *attrib,*head;
    uint32_t len;
    struct busd_object_method method; 
    allocator_t *allocator = busd->allocator;

    obj = (struct busd_object *)allocator_mem_alloc(allocator,
                                                    sizeof(struct busd_object));
    if (obj == NULL) {
        dbg_str(BUS_ERROR,"allocator_mem_alloc");
        return NULL;
    }

    obj->name = (char *)allocator_mem_alloc(allocator,strlen(name));
    strncpy(obj->name,name,strlen(name));
    obj->methods = vector_create(allocator, 0);
    obj->fd      = fd;

    vector_init(obj->methods,sizeof(struct busd_object_method),2);

    head = (blob_attr_t *)blob_get_data(attr);
    len  = blob_get_data_len(attr);

    blob_for_each_attr(attrib, head, len) {
        busd_get_object_method(&method,allocator,attrib);
        vector_push_back(obj->methods,&method);
    }

    busd_dump_object(obj);

    return obj;
}

int busd_reply_add_object(busd_t *busd, int state,char *obj_name, int fd)
{
    bus_reqhdr_t hdr;
    blob_t *blob;
#define BUS_ADD_OBJECT_MAX_BUFFER_LEN 1024
    uint8_t buffer[BUS_ADD_OBJECT_MAX_BUFFER_LEN];
#undef BUS_ADD_OBJECT_MAX_BUFFER_LEN 
    uint32_t buffer_len;
    allocator_t *allocator = busd->allocator;

    dbg_str(BUS_SUC,"busd_reply_lookup_object");
    memset(&hdr,0,sizeof(hdr));

    hdr.type = BUSD_REPLY_ADD_OBJECT;

    blob = blob_create(allocator);
    if (blob == NULL) {
        dbg_str(BUS_WARNNING,"blob_create");
        return -1;
    }
    blob_init(blob);
    blob_add_table_start(blob,(char *)"add_obj_reply"); {
        blob_add_u32(blob, (char *)"state", state);
        blob_add_string(blob, (char *)"object_name", obj_name);
    }
    blob_add_table_end(blob);

    memcpy(buffer,&hdr, sizeof(hdr));
    buffer_len = sizeof(hdr);
    /*
     *dbg_buf(BUS_DETAIL,"object:",(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
     */
    memcpy(buffer + buffer_len,(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
    buffer_len += blob_get_len((blob_attr_t *)blob->head);

    dbg_buf(BUS_DETAIL,"bus send:",buffer,buffer_len);

    write(fd, buffer, buffer_len);  

    return 0;
}

int busd_handle_add_object(busd_t *busd,  blob_attr_t **attr,int fd)
{
    struct busd_object *obj;
    char *obj_name;
    int state = -1;
    dbg_str(BUS_DETAIL,"ubusd_handle_add_object");

    if (attr[BUSD_ID]) {
        dbg_str(BUS_DETAIL,"add object id:%d",blob_get_u32(attr[BUSD_ID]));
    }
    if (attr[BUSD_OBJNAME]) {
        obj_name = blob_get_string(attr[BUSD_OBJNAME]);
        dbg_str(BUS_DETAIL,"add object name:%s",blob_get_string(attr[BUSD_OBJNAME]));
    }
    if (attr[BUSD_METHORDS]) {
        dbg_str(BUS_DETAIL,"add object methods");
        obj = busd_create_bus_object(busd,blob_get_string(attr[BUSD_OBJNAME]), attr[BUSD_METHORDS],fd);
        if (obj != NULL) {
            dbg_str(BUS_DETAIL,"insert obj:%p",obj);
            uint8_t addr_buffer[8];

            addr_to_buffer(obj,addr_buffer);
            hash_map_insert(busd->obj_hmap,blob_get_string(attr[BUSD_OBJNAME]),addr_buffer);
            state = 1;
        }
    }

    busd_reply_add_object(busd,state,obj_name,fd);

    return 0;
}

int busd_reply_lookup_object(busd_t *busd,struct busd_object *obj,int fd)
{
    bus_reqhdr_t hdr;
    blob_t *blob;
#define BUS_ADD_OBJECT_MAX_BUFFER_LEN 1024
    uint8_t buffer[BUS_ADD_OBJECT_MAX_BUFFER_LEN];
#undef BUS_ADD_OBJECT_MAX_BUFFER_LEN 
    uint32_t buffer_len;
    allocator_t *allocator = busd->allocator;

    dbg_str(BUS_DETAIL,"busd_reply_lookup_object");
    memset(&hdr,0,sizeof(hdr));

    hdr.type = BUSD_REPLY_LOOKUP;

    blob = blob_create(allocator);
    if (blob == NULL) {
        dbg_str(BUS_WARNNING,"blob_create");
        return -1;
    }
    blob_init(blob);
    blob_add_table_start(blob,(char *)"lookup_reply"); {
        dbg_str(BUS_DETAIL,"obj_name:%s",obj->name);
        blob_add_string(blob, (char *)"object_name", obj->name);
        blob_add_u32(blob, (char *)"id", 1);
        blob_add_table_start(blob, (char *)"methods"); {
            vector_pos_t pos,next;
            for (   vector_begin(obj->methods, &pos), vector_pos_next(&pos,&next);
                    !vector_pos_equal(&pos,&obj->methods->end);
                    pos = next, vector_pos_next(&pos,&next))
            {
                struct busd_object_method *method; 
                method = (struct busd_object_method *)vector_pos_get_pointer(&pos);
                blob_add_table_start(blob, method->name); {
                }
                blob_add_table_end(blob);
            }
        }
        blob_add_table_end(blob);
    }
    blob_add_table_end(blob);

    memcpy(buffer,&hdr, sizeof(hdr));
    buffer_len = sizeof(hdr);
    /*
     *dbg_buf(BUS_DETAIL,"object:",(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
     */
    memcpy(buffer + buffer_len,(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
    buffer_len += blob_get_len((blob_attr_t *)blob->head);

    dbg_buf(BUS_DETAIL,"bus send:",buffer,buffer_len);

    write(fd, buffer, buffer_len);  

    return 0;
}

int busd_handle_lookup_object(busd_t *busd,  blob_attr_t **attr,int fd)
{
    uint8_t *p = NULL;
    struct busd_object *obj = NULL;
    int ret;

    dbg_str(BUS_DETAIL,"busd_handle_lookup_object");
    if (attr[BUSD_OBJNAME]) {
        hash_map_pos_t pos;
        char *key = blob_get_string(attr[BUSD_OBJNAME]);
        dbg_str(BUS_DETAIL,"lookup object name:%s", key);
        if (key != NULL) {
            ret = hash_map_search(busd->obj_hmap, key ,&pos);
            if (ret > 0) {
                dbg_str(BUS_DETAIL,"obj addr:%p",obj);
                p = (uint8_t *)hash_map_pos_get_pointer(&pos);
                obj = (struct busd_object *)buffer_to_addr(p);

                dbg_str(BUS_DETAIL,"obj addr:%p",obj);
                busd_dump_object(obj);
                busd_reply_lookup_object(busd,obj,fd);
            }
        }
    }

    return 0;
}

int busd_forward_invoke(busd_t *busd, int src_fd,int dest_fd,char *obj_name, char *method,int argc, blob_attr_t *args)
{
    bus_reqhdr_t hdr;
    blob_t *blob;
#define BUS_ADD_OBJECT_MAX_BUFFER_LEN 1024
    uint8_t buffer[BUS_ADD_OBJECT_MAX_BUFFER_LEN];
#undef BUS_ADD_OBJECT_MAX_BUFFER_LEN 
    uint32_t buffer_len;
    allocator_t *allocator = busd->allocator;

    dbg_str(BUS_SUC,"busd_forward_invoke,argc=%d",argc);
    memset(&hdr,0,sizeof(hdr));

    hdr.type = BUSD_FORWARD_INVOKE;

    blob = blob_create(allocator);
    if (blob == NULL) {
        dbg_str(BUS_WARNNING,"blob_create");
        return -1;
    }
    blob_init(blob);
    blob_add_table_start(blob,(char *)"forword_invoke"); {
        blob_add_u32(blob, (char *)"source_fd", src_fd);
        blob_add_u32(blob, (char *)"destination_fd", dest_fd);
        blob_add_string(blob, (char *)"object_name", obj_name);
        blob_add_string(blob, (char *)"invoke_method", method);
        blob_add_u8(blob, (char *)"invoke_argc", argc);
        blob_catenate(blob, args);
    }
    blob_add_table_end(blob);

    memcpy(buffer,&hdr, sizeof(hdr));
    buffer_len = sizeof(hdr);
    /*
     *dbg_buf(BUS_DETAIL,"object:",(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
     */
    memcpy(buffer + buffer_len,(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
    buffer_len += blob_get_len((blob_attr_t *)blob->head);

    dbg_buf(BUS_DETAIL,"bus send:",buffer,buffer_len);

    write(dest_fd, buffer, buffer_len);  

    return 0;

}

int busd_handle_invoke_method(busd_t *busd,  blob_attr_t **attr,int fd)
{
    struct busd_object *obj = NULL;
    hash_map_pos_t pos;
    char *key;
    int ret;
    uint8_t *p = NULL;
    char *method;
    char *obj_name = NULL;
    blob_attr_t *args = NULL;
    int argc = 0; 

    dbg_str(BUS_SUC,"busd_handle_invoke_method");

    if (attr[BUSD_INVOKE_KEY]) {
        obj_name = blob_get_string(attr[BUSD_INVOKE_KEY]);
        dbg_str(BUS_DETAIL,"invoke key:%s",obj_name);
        key = blob_get_string(attr[BUSD_INVOKE_KEY]);
        if (key != NULL) {
            ret = hash_map_search(busd->obj_hmap, key ,&pos);
            if (ret > 0) {
                dbg_str(BUS_DETAIL,"obj addr:%p",obj);
                p = (uint8_t *)hash_map_pos_get_pointer(&pos);
                obj = (struct busd_object *)buffer_to_addr(p);

                dbg_str(BUS_DETAIL,"obj addr:%p",obj);
                busd_dump_object(obj);
            }
        }
    }

    if (attr[BUSD_INVOKE_METHORD]) {
        dbg_str(BUS_DETAIL,"invoke method:%s",blob_get_string(attr[BUSD_INVOKE_METHORD]));
        method = blob_get_string(attr[BUSD_INVOKE_METHORD]);
    }
    if (attr[BUSD_INVOKE_ARGC]) {
        argc = blob_get_u8(attr[BUSD_INVOKE_ARGC]); 
        dbg_str(BUS_DETAIL,"invoke argc:%d",argc);
    }
    if (attr[BUSD_INVOKE_ARGS]) {
        dbg_str(BUS_DETAIL,"invoke args");
        args = attr[BUSD_INVOKE_ARGS];
    }

    busd_forward_invoke(busd, fd, obj->fd,obj_name, method,argc, args);

    return 0;
}

int busd_reply_invoke(busd_t *busd,char *obj_name,char *method,int state,uint8_t *opaque,int opaque_len, int source_fd)
{
    bus_reqhdr_t hdr;
    blob_t *blob;
#define BUS_ADD_OBJECT_MAX_BUFFER_LEN 1024
    uint8_t buffer[BUS_ADD_OBJECT_MAX_BUFFER_LEN];
#undef BUS_ADD_OBJECT_MAX_BUFFER_LEN 
    uint32_t buffer_len;
    allocator_t *allocator = busd->allocator;

    dbg_str(BUS_SUC,"busd_reply_invoke");
    memset(&hdr,0,sizeof(hdr));

    hdr.type = BUSD_REPLY_INVOKE;

    blob = blob_create(allocator);
    if (blob == NULL) {
        dbg_str(BUS_WARNNING,"blob_create");
        return -1;
    }
    blob_init(blob);
    blob_add_table_start(blob,(char *)"invoke_reply"); {
        blob_add_u32(blob, (char *)"state", state);
        blob_add_buffer(blob, (char *)"opaque", opaque, opaque_len);
        blob_add_string(blob, (char *)"object_name", obj_name);
        blob_add_string(blob, (char *)"invoke_method", method);
    }
    blob_add_table_end(blob);

    memcpy(buffer,&hdr, sizeof(hdr));
    buffer_len = sizeof(hdr);
    /*
     *dbg_buf(BUS_DETAIL,"object:",(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
     */
    memcpy(buffer + buffer_len,(uint8_t *)blob->head,blob_get_len((blob_attr_t *)blob->head));
    buffer_len += blob_get_len((blob_attr_t *)blob->head);

    dbg_buf(BUS_DETAIL,"bus send:",buffer,buffer_len);

    write(source_fd, buffer, buffer_len);  

    return 0;
}

int busd_handle_forward_invoke_reply(busd_t *busd,  blob_attr_t **attr,int fd)
{
    int state;
    char *method;
    char *obj_name;
    int src_fd;
    uint8_t *buffer = NULL;
    int buffer_len = 0;

    dbg_str(BUS_SUC,"busd_handle_forward_invoke_reply");

    if (attr[BUSD_STATE]) {
        state = blob_get_u32(attr[BUSD_STATE]); 
        dbg_str(BUS_DETAIL,"forward invoke reply state=%d",state);
    }
    if (attr[BUSD_INVOKE_METHORD]) {
        method = blob_get_string(attr[BUSD_INVOKE_METHORD]); 
        dbg_str(BUS_DETAIL,"method:%s",method);
    }
    if (attr[BUSD_OBJNAME]) {
        obj_name = blob_get_string(attr[BUSD_OBJNAME]); 
        dbg_str(BUS_DETAIL,"obj_name:%s",obj_name);
    }
    if (attr[BUSD_INVOKE_SRC_FD]) {
        src_fd = blob_get_u32(attr[BUSD_INVOKE_SRC_FD]); 
        dbg_str(BUS_DETAIL,"source fd=%d",src_fd);
    }
    if (attr[BUSD_OPAQUE]) {
        buffer_len = blob_get_buffer(attr[BUSD_OPAQUE],&buffer);
        dbg_buf(BUS_DETAIL,"busd_handle_forward_invoke_reply,buffer:",buffer,buffer_len);
    }

    busd_reply_invoke(busd,obj_name,method,state,buffer,buffer_len,src_fd);

    return 0;
}

static busd_cmd_callback handlers[__BUS_REQ_LAST] = {
    [BUS_REQ_ADD_OBJECT]       = busd_handle_add_object,
    [BUS_REQ_LOOKUP]           = busd_handle_lookup_object,
    [BUS_REQ_INVOKE]           = busd_handle_invoke_method,
    [BUS_REPLY_FORWARD_INVOKE] = busd_handle_forward_invoke_reply,
};

static int busd_process_receiving_data_callback(void *task)
{
    server_data_task_t *t = (server_data_task_t *)task;;
    bus_reqhdr_t *hdr;
    blob_attr_t *blob_attr;
    blob_attr_t *tb[__BUSD_MAX];
    busd_cmd_callback cb = NULL;
    server_t *server = t->server;
    busd_t *busd = (busd_t *)server->opaque;
    int len;

    hdr = (bus_reqhdr_t *)t->buffer;
    blob_attr = (blob_attr_t *)(t->buffer + sizeof(bus_reqhdr_t));

    if (hdr->type > __BUS_REQ_LAST) {
        dbg_str(BUS_WARNNING,"busd receive err proto type");
        return -1;
    } 

    cb = handlers[hdr->type];

    len = blob_get_data_len(blob_attr);
    blob_attr =(blob_attr_t*) blob_get_data(blob_attr);
    dbg_buf(BUS_DETAIL,"rcv oject:",(uint8_t *)blob_attr,len);

    blob_parse_to_attr(busd_policy,
                       ARRAY_SIZE(busd_policy),
                       tb,
                       blob_attr,
                       len);

    cb(busd,tb,t->fd);

    return 0;
}

busd_t *busd_create(allocator_t *allocator,
                    char *server_host,
                    char *server_srv,
                    char *socket_type)
{
    busd_t *busd;
    
    dbg_str(BUS_DETAIL,"bus_daemon_create");
    busd = busd_alloc(allocator);

    busd_set(busd, (char *)"server_sk_type", socket_type, 0);

    busd_init(busd,//busd_t *busd,
              server_host,//char *server_host,
              server_srv,//char *server_srv,
              busd_process_receiving_data_callback);
}

