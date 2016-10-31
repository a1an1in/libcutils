#ifndef __BUS_H__
#define __BUS_H__

#include <libipc/net/client.h>
#include <libblob/msgblob.h>
#include <libblob/blob.h>

struct bus_s;
typedef int (*bus_handler_t)(struct bus_s *bus,
							 struct blob_attr *msg);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#define BUS_METHOD(_name, _handler, _policy)\
{\
	.name = _name,\
	.handler = _handler,\
	.policy = (struct msgblob_policy *)_policy,\
	.n_policy = ARRAY_SIZE(_policy),\
}

typedef struct bus_reqhdr {
	uint8_t version;
	uint8_t type;
	uint32_t s_id;
	uint32_t d_id;
}__attribute__((packed)) bus_reqhdr_t;

enum bus_req_type {
	/* initial server message */
	BUS_REQ_HELLO,
	/* generic command response */
	BUS_REQ_STATUS,
	/* data message response */
	BUS_REQ_DATA,
	/* ping request */
	BUS_REQ_PING,
	/* look up one or more objects */
	BUS_REQ_LOOKUP,
	/* invoke a method on a single object */
	BUS_REQ_INVOKE,
	BUS_REQ_ADD_OBJECT,
	BUS_REQ_REMOVE_OBJECT,
	/*
	 * subscribe/unsubscribe to object notifications
	 * The unsubscribe message is sent from busd when
	 * the object disappears
	 */
	BUS_REQ_SUBSCRIBE,
	BUS_REQ_UNSUBSCRIBE,
	/*
	 * send a notification to all subscribers of an object.
	 * when sent from the server, it indicates a subscription
	 * status change
	 */
	BUS_REQ_NOTIFY,
	/* must be last */
	__BUS_REQ_LAST,
};

struct bus_method {
	char *name;
	bus_handler_t handler;
	struct msgblob_policy *policy;
	int n_policy;
};

struct bus_object {
	char *name;
	uint32_t id;
	char *path;
	struct bus_method *methods;
	int n_methods;
};

typedef struct bus_s{
    allocator_t *allocator;
    client_t *client;
    char *client_sk_type;
    char *server_host;
    char *server_srv;
}bus_t;


bus_t * bus_create(allocator_t *allocator);
int bus_init(bus_t *bus,
             char *server_host,
             char *server_srv,
             int (*process_client_task_cb)(client_task_t *task));
int bus_send(bus_t *bus,
			 void *buf,
			 size_t buf_len);

int bus_add_object(bus_t *bus,struct bus_object *obj);
#endif