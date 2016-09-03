#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <libconcurrent/concurrent.h>

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
	void *opaque;
	concurrent_master_t *master;
}client_t;
#endif
