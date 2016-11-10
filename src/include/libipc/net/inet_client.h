#ifndef __INET_CLIENT_H__
#define __INET_CLIENT_H__

#include <libconcurrent/concurrent.h>
#include <libconcurrent/io_user.h>

/*
 *enum socktype_e{
 *    SOCKTYPE_UDP = 0,
 *    SOCKTYPE_TCP,
 *    SOCKTYPE_UNIX
 *};
 */

/*
 *struct user_s;
 */
typedef struct io_user_s client_t;

typedef struct client_task_s{
#define MAX_TASK_BUFFER_LEN 1024 * 20
	uint32_t fd;
	char key[10];
	struct event *event;
	allocator_t *allocator;
	concurrent_slave_t *slave;
	uint8_t buffer[MAX_TASK_BUFFER_LEN];
	uint32_t buffer_len;
	client_t *client;
#undef MAX_TASK_BUFFER_LEN
}client_task_t;


client_t *udp_iclient(allocator_t *allocator,
					 char *host,
					 char *client_port,
					 int (*process_task_cb)(client_task_t *task),
					 void *opaque);

client_t *tcp_iclient(allocator_t *allocator,
					 char *server_ip,
					 char *server_port,
					 int (*process_task_cb)(client_task_t *task),
					 void *opaque);

int iclient_destroy(client_t *client);

int udp_iclient_send(client_t *client,const void *buf,size_t nbytes,int flags,
		const struct sockaddr *destaddr,socklen_t destlen);

int tcp_iclient_send(client_t *client,const void *buf,size_t nbytes,int flags);
#endif
