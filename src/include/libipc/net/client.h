#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <libipc/net/inet_client.h>
#include <libipc/net/unix_client.h>

#define CLIENT_TYPE_TCP_INET "tcp_iclient_type"
#define CLIENT_TYPE_UDP_INET "udp_iclient_type"
#define CLIENT_TYPE_TCP_UNIX "tcp_uclient_type"
#define CLIENT_TYPE_UDP_UNIX "udp_uclient_type"

client_t *client(allocator_t *allocator,
                 char *type,
                 char *id_str,
                 char *serve_no,
                 int (*process_task_cb)(client_task_t *task),
                 void *opaque);
int client_send(client_t *client,const void *buf,size_t nbytes,int flags,
        char *dest_id_str, char *dest_srv_str);
int client_destroy(client_t *client);

void test_client_recv_of_inet_udp();
void test_client_recv_of_unix_udp();

#endif
