#ifndef __SERVER_H__
#define __SERVER_H__

#include <libipc/net/inet_server.h>
#include <libipc/net/unix_server.h>

#define TCP_ISERVER_TYPE "tcp_iserver_type"
#define TCP_USERVER_TYPE "tcp_userver_type"

server_t * server(allocator_t *allocator,
                  char *type,
                  char *host_ip, 
                  char *server_port,
                  int (*process_task_cb)(void *task),
                  void *opaque);
#endif
