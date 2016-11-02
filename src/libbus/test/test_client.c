#include <libipc/net/client.h>
#include <libbus/bus.h>

static int bus_process_receiving_data_callback(client_task_t *task)
{
	dbg_str(DBG_DETAIL,"process_task begin,unix client recv");
	dbg_buf(DBG_VIP,"task buffer:",task->buffer,task->buffer_len);
	dbg_str(DBG_DETAIL,"%s",task->buffer);
	dbg_str(DBG_DETAIL,"process_task end");

    return 0;
}

void test_bus_client()
{
    allocator_t *allocator = allocator_get_default_alloc();
    bus_t *bus;
    char *server_host = "bus_server_path";
    char *server_srv = NULL;
	char buf[1024] = "hello world!";
	int buf_len = strlen(buf);
    
    dbg_str(DBG_DETAIL,"test_busd_daemon");
    bus = bus_create(allocator);

    bus_init(bus,//busd_t *busd,
             server_host,//char *server_host,
             server_srv,//char *server_srv,
             bus_process_receiving_data_callback);

    bus_lookup(bus, "test");
	
}
