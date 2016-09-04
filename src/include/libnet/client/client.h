#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <libconcurrent/concurrent.h>

/*
 *enum socktype_e{
 *    SOCKTYPE_UDP = 0,
 *    SOCKTYPE_TCP,
 *    SOCKTYPE_UNIX
 *};
 */
typedef struct client_task_s{
#define MAX_TASK_BUFFER_LEN 1024
	uint32_t fd;
	char key[10];
	struct event *event;
	allocator_t *allocator;
	concurrent_slave_t *slave;
	uint8_t buffer[MAX_TASK_BUFFER_LEN];
	uint32_t buffer_len;
	struct client_s *client;
#undef MAX_TASK_BUFFER_LEN
}client_task_t;
typedef struct client_s{
	/*
	 *client_task_t *task;
	 */
	struct event event;
	int client_fd;
	void (*slave_work_function)(concurrent_slave_t *slave,void *arg);
	void (*client_event_handler)(int fd, short event, void *arg);
	int (*process_task_cb)(client_task_t *task);
	void *opaque;
	concurrent_master_t *master;
	allocator_t *allocator;
	void *local_sockaddr;
	void *remote_sockaddr;
	uint8_t socktype;
}client_t;

client_t *client(char *host,
				 char *client_port,
				 int family,
				 int socktype,
				 int protocol,
				 int (*process_task_cb)(client_task_t *task),
				 /*
				  *void (*slave_work_function)(concurrent_slave_t *slave,void *arg),
				  */
				 void *opaque);
int client_send(client_t *client,const void *buf,size_t nbytes,int flags,
		const struct sockaddr *destaddr,socklen_t destlen);
int client_release_task(client_task_t *task);
int test_client_recieve();
int test_client_send();
#endif
