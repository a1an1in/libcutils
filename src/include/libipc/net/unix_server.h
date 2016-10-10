#ifndef __UNIX_SK_SERVER_H__
#define __UNIX_SK_SERVER_H__
#include <libconcurrent/concurrent.h>
#include <libipc/net/inet_server.h>

typedef struct server_task_s{
	uint32_t fd;
	char key[10];
	struct event *event;
	allocator_t *allocator;
	concurrent_slave_t *slave;
    server_t *server;
}server_task_t;

server_t * tcp_userver(allocator_t *allocator,
        char *server_un_path,
        int (*process_task_cb)(void *task),
        void *opaque);
int tcp_userver_destroy(server_t *server);
int test_tcp_userver();
#endif
