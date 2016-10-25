
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <libblob/msgblob.h>
#include <libblob/blob.h>

#include "libubus.h"
#include "ubusmsg.h"

struct blob_buf b;

#define UBUS_MAX_MSGLEN 1024
/*
 *int ubus_connect_ctx(struct ubus_context *ctx, const char *path)
 *{
 *    memset(ctx, 0, sizeof(*ctx));
 *
 *    ctx->sock.fd = -1;
 *    ctx->sock.cb = ubus_handle_data;
 *    ctx->connection_lost = ubus_default_connection_lost;
 *    ctx->pending_timer.cb = ubus_process_pending_msg;
 *
 *    ctx->msgbuf.data = calloc(UBUS_MSG_CHUNK_SIZE, sizeof(char));
 *    if (!ctx->msgbuf.data)
 *        return -1;
 *    ctx->msgbuf_data_len = UBUS_MSG_CHUNK_SIZE;
 *
 *    INIT_LIST_HEAD(&ctx->requests);
 *    INIT_LIST_HEAD(&ctx->pending);
 *    avl_init(&ctx->objects, ubus_cmp_id, false, NULL);
 *    if (ubus_reconnect(ctx, path)) {
 *        free(ctx->msgbuf.data);
 *        return -1;
 *    }
 *
 *    return 0;
 *}
 */
int __hidden ubus_send_msg(struct ubus_context *ctx, uint32_t seq,
			   struct blob_attr *msg, int cmd, uint32_t peer, int fd)
{
	struct ubus_msghdr hdr;
	int ret = 0;
    char send_buf[1024* 4];

	hdr.version = 0;
	hdr.type = cmd;
	hdr.seq = seq;
	hdr.peer = peer;

    memcpy(send_buf,&hdr,sizeof(hdr));
    memcpy(send_buf + sizeof(hdr),msg, blob_raw_len(msg));

    client_send(ctx->client,
            send_buf,//const void *buf,
            sizeof(hdr) + blob_raw_len(msg),//size_t nbytes,
            0,//int flags,
            ctx->client_id_str,//char *dest_id_str, 
            ctx->client_server_str);//char *dest_srv_str)

	return ret;
}
int ubus_send_reply(struct ubus_context *ctx, struct ubus_request_data *req,
		    struct blob_attr *msg)
{
	int ret;

	blob_buf_init(&b, 0);
	blob_put_u32(&b, UBUS_ATTR_OBJID, req->object);
	blob_put(&b, UBUS_ATTR_DATA, blob_data(msg), blob_len(msg));
	ret = ubus_send_msg(ctx, req->seq, b.head, UBUS_MSG_DATA, req->peer, -1);
	if (ret < 0)
		return UBUS_STATUS_NO_DATA;

	return 0;
}
int __hidden ubus_start_request(struct ubus_context *ctx, struct ubus_request *req,
				struct blob_attr *msg, int cmd, uint32_t peer)
{
	memset(req, 0, sizeof(*req));

	if (msg && blob_pad_len(msg) > UBUS_MAX_MSGLEN)
		return -1;

	INIT_LIST_HEAD(&req->list);
	INIT_LIST_HEAD(&req->pending);
	req->ctx = ctx;
	req->peer = peer;
	req->seq = ++ctx->request_seq;
	return ubus_send_msg(ctx, req->seq, msg, cmd, peer, -1);
}
int ubus_add_object(struct ubus_context *ctx, struct ubus_object *obj)
{
	struct ubus_request req;
	int ret;

	blob_buf_init(&b, 0);

	if (obj->name && obj->type) {
		blob_put_string(&b, UBUS_ATTR_OBJPATH, obj->name);

		if (obj->type->id)
			blob_put_u32(&b, UBUS_ATTR_OBJTYPE, obj->type->id);
        /*
		 *else if (!ubus_push_object_type(obj->type))
		 *    return UBUS_STATUS_INVALID_ARGUMENT;
         */
	}

	if (ubus_start_request(ctx, &req, b.head, UBUS_MSG_ADD_OBJECT, 0) < 0)
		return UBUS_STATUS_INVALID_ARGUMENT;

    /*
	 *ret = ubus_complete_request(ctx, &req, 0);
	 *if (ret)
	 *    return ret;
     */

	if (!obj->id)
		return UBUS_STATUS_NO_DATA;

	return 0;
}



struct ubus_context * ubus_client_create(allocator_t *allocator)
{
	struct ubus_context *ctx;
    ctx = (struct ubus_context *)allocator_mem_alloc(allocator,sizeof(struct ubus_context));
    if(ctx == NULL) return NULL;
    ctx->allocator = allocator;

    return ctx;
}
struct ubus_context *ubus_client_set(struct ubus_context *ctx,void *key,void *value,uint32_t value_len)
{
}

static int process_ubus_client_task_callback(client_task_t *task)
{
    dbg_str(DBG_DETAIL,"process_ubus_client_task_callback");
    struct ubus_msghdr hdr;
    
    memcpy(&hdr,task->buffer,sizeof(hdr));

    /*
	 *switch(hdr.type) {
	 *case UBUS_MSG_STATUS:
	 *case UBUS_MSG_DATA:
	 *    ubus_process_req_msg(ctx, buf, fd);
	 *    break;
	 *case UBUS_MSG_INVOKE:
	 *case UBUS_MSG_UNSUBSCRIBE:
	 *case UBUS_MSG_NOTIFY:
	 *    if (ctx->stack_depth) {
	 *        ubus_queue_msg(ctx, buf);
	 *        break;
	 *    }
	 *    ubus_process_obj_msg(ctx, buf);
	 *    break;
	 *}
     */

}
struct ubus_context *ubus_client_connect(struct ubus_context *ctx)
{

    if(1){
        ctx->client = client(ctx->allocator,
                             CLIENT_TYPE_UDP_UNIX,
                             ctx->client_id_str,//char *host,
                             NULL,//char *client_port,
                             process_ubus_client_task_callback,
                             NULL);
    }else{
        dbg_str(DBG_WARNNING,"not support now");
        ctx->client = NULL;
    }
    return ctx;
}

void ubus_client_destroy(struct ubus_context *ctx)
{
}
