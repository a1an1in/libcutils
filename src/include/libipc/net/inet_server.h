#ifndef __INET_SERVER_H__
#define __INET_SERVER_H__

#include <libconcurrent/io_user.h>
#include <libipc/net/server_struct.h>

server_t * tcp_iserver(allocator_t *allocator,
                       char *host_ip, char *server_port,
                       int (*process_task_cb)(void *task),
                       void *opaque);
int tcp_iserver_destroy(server_t *server);
int test_iserver();

#endif
