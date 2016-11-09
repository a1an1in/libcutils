#ifndef __SERVER_STRUCT_H__
#define __SERVER_STRUCT_H__

#include <libconcurrent/io_user.h>

typedef struct io_user_s server_t;

typedef struct server_task_s{
	uint32_t fd;
	char key[10];
	struct event *event;
	allocator_t *allocator;
	concurrent_slave_t *slave;
    server_t *server;
}server_task_t;

typedef struct server_data_task_s{
#define MAX_TASK_BUFFER_LEN 1024
	uint32_t fd;
	char key[10];
	struct event *event;
	allocator_t *allocator;
	concurrent_slave_t *slave;
    server_t *server;
	uint8_t buffer[MAX_TASK_BUFFER_LEN];
	int buffer_len;
#undef MAX_TASK_BUFFER_LEN
}server_data_task_t;

#endif
