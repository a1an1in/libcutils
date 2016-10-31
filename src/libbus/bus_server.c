#include <libipc/net/client.h>
#include <libbus/bus.h>
#include <unistd.h>

enum {
	HELLO_ID,
	HELLO_MSG,
	__HELLO_MAX
};

static const struct msgblob_policy hello_policy[] = {
	[HELLO_ID] = { .name = "id", .type = MSGBLOB_TYPE_INT32 },
	[HELLO_MSG] = { .name = "msg", .type = MSGBLOB_TYPE_STRING },
};

static int test_hello(struct bus_s *bus,
		      		  struct blob_attr *msg)
{
	struct blob_attr *tb[__HELLO_MAX];

    printf("run test hello\n");
	msgblob_parse(hello_policy, ARRAY_SIZE(hello_policy), tb, blob_data(msg), blob_len(msg));

    /*
	 *if (tb[HELLO_MSG])
	 *    msgstr = msgblob_data(tb[HELLO_MSG]);
     */

	return 0;
}

enum {
	SET_ID,
	SET_MSG,
	__SET_MAX
};

static const struct msgblob_policy set_policy[] = {
	[SET_ID] = { .name = "id", .type = MSGBLOB_TYPE_INT32 },
	[SET_MSG] = { .name = "msg", .type = MSGBLOB_TYPE_STRING },
};

static int test_set(struct bus_s *bus,
		      		  struct blob_attr *msg)
{
	struct blob_attr *tb[__SET_MAX];

    printf("run set set\n");
	msgblob_parse(set_policy, ARRAY_SIZE(set_policy), tb, blob_data(msg), blob_len(msg));

    /*
	 *if (tb[SET_MSG])
	 *    msgstr = msgblob_data(tb[SET_MSG]);
     */

	return 0;
}


static const struct bus_method test_methods[] = {
	BUS_METHOD("hello", test_hello, hello_policy),
	BUS_METHOD("set", test_set, set_policy),
};

static struct bus_object test_object = {
	.name = (char *)"test",
	.methods = (struct bus_method *)test_methods,
	.n_methods = ARRAY_SIZE(test_methods),
};

static int bus_process_receiving_data_callback(client_task_t *task)
{
	dbg_str(DBG_DETAIL,"process_task begin,unix client recv");
	dbg_buf(DBG_VIP,"task buffer:",task->buffer,task->buffer_len);
	dbg_str(DBG_DETAIL,"process_task end");

    return 0;
}

void test_bus_server()
{
    allocator_t *allocator = allocator_get_default_alloc();
    bus_t *bus;
    char *server_host = (char *)"bus_server_path";
    char *server_srv = NULL;
	char buf[1024] = "hello world!";
	int buf_len = strlen(buf);
    
    dbg_str(DBG_DETAIL,"test_bus_server");
    bus = bus_create(allocator);

    bus_init(bus,//busd_t *busd,
             server_host,//char *server_host,
             server_srv,//char *server_srv,
             bus_process_receiving_data_callback);

    dbg_str(DBG_DETAIL,"bus add object");
	bus_add_object(bus,&test_object);
	
	/*
	 *bus_send(bus, buf,buf_len);
	 *sleep(1);
	 *bus_send(bus, buf,buf_len);
	 *sleep(1);
	 *bus_send(bus, buf,buf_len);
	 *sleep(1);
	 *bus_send(bus, buf,buf_len);
	 *sleep(1);
	 *bus_send(bus, buf,buf_len);
	 *sleep(1);
	 */
	while(1) sleep(1);
}


