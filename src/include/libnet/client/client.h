#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <libconcurrent/concurrent.h>

typedef struct client_task_s{
	uint32_t fd;
	char key[10];
	struct event *event;
	allocator_t *allocator;
	concurrent_slave_t *slave;
}client_task_t;
typedef struct client_s{
	client_task_t *task;
	struct event *event;
	int client_fd;
}client_t;
#endif
