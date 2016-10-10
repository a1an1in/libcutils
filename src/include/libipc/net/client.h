#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <libipc/net/inet_client.h>
#include <libipc/net/unix_client.h>

#define TCP_ICLIENT_TYPE "tcp_iclient_type"
#define UDP_ICLIENT_TYPE "udp_iclient_type"
#define TCP_UCLIENT_TYPE "tcp_uclient_type"
#define UDP_UCLIENT_TYPE "udp_uclient_type"

client_t *client(allocator_t *allocator,
                 char *id_str,
                 char *serve_no,
                 char *type,
                 int (*process_task_cb)(client_task_t *task),
                 void *opaque);
int client_send(client_t *client,const void *buf,size_t nbytes,int flags,
        char *dest_id_str, char *dest_srv_str);
int client_destroy(client_t *client);

void test_client_recv_of_inet_udp();
void test_client_recv_of_unix_udp();

#endif
