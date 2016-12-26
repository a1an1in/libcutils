#ifndef __SERVER_H__
#define __SERVER_H__

#include <libipc/net/inet_server.h>
#include <libipc/net/unix_server.h>

#define SERVER_TYPE_INET_TCP "inet_tcp_server_type"
#define SERVER_TYPE_UNIX_TCP "tcp_userver_type"

server_t * server(allocator_t *allocator,
                  char *type,
                  char *host_ip, 
                  char *server_port,
                  int (*process_task_cb)(void *task),
                  void *opaque);
int server_destroy(server_t *server);

int test_server_of_unix();
int test_server_of_inet();
int test_tcp_userver();
#endif
