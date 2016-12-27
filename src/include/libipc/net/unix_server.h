#ifndef __UNIX_SK_SERVER_H__
#define __UNIX_SK_SERVER_H__
#include <libconcurrent/concurrent.h>
#include <libipc/net/server_struct.h>

server_t * unix_tcp_server(allocator_t *allocator,
                           char *server_un_path,
                           int (*process_task_cb)(void *task),
                           void *opaque);
int unix_tcp_server_destroy(server_t *server);
int test_unix_tcp_server();

#endif
