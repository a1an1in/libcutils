/*
 * Copyright (C) 2011-2014 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <unistd.h>
#include <signal.h>

#include "libblob/msgblob.h"
#include "libubus.h"

static struct ubus_context *ctx;
static struct blob_buf b;

extern int ubus_send_reply(struct ubus_context *ctx, struct ubus_request_data *req, struct blob_attr *msg);
extern int ubus_add_object(struct ubus_context *ctx, struct ubus_object *obj);

enum {
	HELLO_ID,
	HELLO_MSG,
	__HELLO_MAX
};

static const struct msgblob_policy hello_policy[] = {
	[HELLO_ID] = { .name = "id", .type = MSGBLOB_TYPE_INT32 },
	[HELLO_MSG] = { .name = "msg", .type = MSGBLOB_TYPE_STRING },
};

static int test_hello(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__HELLO_MAX];
    /*
	 *const char *format = "%s received a message: %s";
     */
    /*
	 *const char *msgstr = "(unknown)";
     */
    struct ubus_request_data new_req;

    printf("run test hello\n");
	msgblob_parse(hello_policy, ARRAY_SIZE(hello_policy), tb, blob_data(msg), blob_len(msg));

    /*
	 *if (tb[HELLO_MSG])
	 *    msgstr = msgblob_data(tb[HELLO_MSG]);
     */

    blob_buf_init(&b, 0);
    msgblob_add_string(&b, "message", "hello world");
    ubus_send_reply(ctx, &new_req, b.head);

	return 0;
}


static const struct ubus_method test_methods[] = {
	UBUS_METHOD("hello", test_hello, hello_policy),
};

static struct ubus_object_type test_object_type =
	UBUS_OBJECT_TYPE("test", test_methods);

static struct ubus_object test_object = {
	.name = "test",
	.type = &test_object_type,
	.methods = test_methods,
	.n_methods = ARRAY_SIZE(test_methods),
};

static void server_main(void)
{
	int ret;

	ret = ubus_add_object(ctx, &test_object);
}

int ubus_client_main(int argc, char **argv)
{
	server_main();

	return 0;
}
