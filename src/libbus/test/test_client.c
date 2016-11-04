#include <libipc/net/client.h>
#include <libbus/bus.h>

void test_bus_client()
{
    allocator_t *allocator = allocator_get_default_alloc();
    bus_t *bus;
    char *args[2] = {"abc","hello world!"};
    char *server_host = "bus_server_path";
    char *server_srv = NULL;
	char buf[1024] = "hello world!";
	int buf_len = strlen(buf);
    
    dbg_str(DBG_DETAIL,"test_bus_client");

    bus = bus_client_create(allocator,
                            server_host,
                            server_srv);
    /*
     *bus_lookup(bus, "test");
     */

    bus_invoke(bus,"test", "hello",2, args);
	
}
