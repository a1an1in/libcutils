#ifndef __SERVER_H__
#define __SERVER_H__

#include <libipc/net/inet_server.h>
#include <libipc/net/unix_server.h>

#define SERVER_TYPE_TCP_INET "tcp_iserver_type"
#define SERVER_TYPE_TCP_UNIX "tcp_userver_type"

server_t * server(allocator_t *allocator,
                  char *type,
                  char *host_ip, 
                  char *server_port,
                  int (*process_task_cb)(void *task),
                  void *opaque);
int server_destroy(server_t *server);

int test_server_of_unix();
int test_server_of_inet();
#endif
