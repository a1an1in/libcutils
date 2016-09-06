#ifndef __USER_H__
#define __USER_H__

#include <libconcurrent/concurrent.h>


typedef struct user_s{
	struct event event;
	int user_fd;
	void (*slave_work_function)(concurrent_slave_t *slave,void *arg);
	void (*user_event_handler)(int fd, short event, void *arg);
	int (*process_task_cb)(void *task);
	void *opaque;
	concurrent_master_t *master;
	allocator_t *allocator;
	void *local_sockaddr;
	void *remote_sockaddr;
	uint8_t user_type;
}user_t;


user_t *user(
		allocator_t *allocator,
		int user_fd,
		uint8_t user_type,
		void (*user_event_handler)(int fd, short event, void *arg),
		void (*slave_work_function)(concurrent_slave_t *slave,void *arg),
		int (*process_task_cb)(void *task),
		void *opaque);

#endif
