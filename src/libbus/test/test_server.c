#include <libipc/net/client.h>
#include <libbus/bus.h>
#include <unistd.h>

enum {
	HELLO_ID,
	HELLO_MSG,
	__HELLO_MAX
};

static const struct blob_policy_s hello_policy[] = {
	[HELLO_ID]  = { .name = "arg", .type = BLOB_TYPE_INT32 },
	[HELLO_MSG] = { .name = "def", .type = BLOB_TYPE_STRING },
};

static int test_hello(struct bus_s *bus,
                      int argc,
		      		  struct blob_attr_s **args)
{
    printf("run test hello\n");

	return 0;
}

enum {
	SET_ID,
	SET_MSG,
	__SET_MAX
};

static const struct blob_policy_s set_policy[] = {
	[SET_ID]  = { .name = "hijklmn", .type = BLOB_TYPE_INT32 },
	[SET_MSG] = { .name = "opqrst", .type = BLOB_TYPE_STRING },
};

static int test_set(struct bus_s *bus,
                    int argc,
		      		struct blob_attr_s **args)
{
    printf("run set set\n");
	return 0;
}

static const struct bus_method test_methods[] = {
	BUS_METHOD("hello", test_hello, hello_policy),
	BUS_METHOD("set", test_set, set_policy),
};

static struct bus_object test_object = {
	.name      = (char *)"test",
	.methods   = (struct bus_method *)test_methods,
	.n_methods = ARRAY_SIZE(test_methods),
};

void test_bus_server()
{
    allocator_t *allocator = allocator_get_default_alloc();
    bus_t *bus;
    char *server_host = (char *)"bus_server_path";
    char *server_srv  = NULL;
	char buf[1024]    = "hello world!";
	int buf_len       = strlen(buf);
    
    bus = bus_client_create(allocator,
                            server_host,
                            server_srv);

    dbg_str(DBG_DETAIL,"bus add object");
	bus_add_object(bus,&test_object);

	while(1) sleep(1);
}


