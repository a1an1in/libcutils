#ifndef __CONCURRENT_H__
#define __CONCURRENT_H__

#include <event.h>
#include <libdbg/debug.h>
#include "libconcurrent/concurrent.h"
#include "libdata_structure/link_list.h"
#include "libdata_structure/hash_list.h"

enum SERVER_WORK_TYPE{
	SERVER_WORK_TYPE_THREAD = 0,
	SERVER_WORK_TYPE_PROCESS
};


typedef struct concurrent_task_admin_s{
	sync_lock_t admin_lock;
	uint8_t admin_lock_type;
	uint8_t hmap_lock_type;
	hash_map_t *hmap;
	uint32_t task_count;
	allocator_t *allocator;
	uint8_t key_size;
	uint32_t data_size;
	uint32_t bucket_size;
}concurrent_task_admin_t;

typedef struct concurrent_slave_s{
	struct event_base *event_base;
	struct event message_event;
	allocator_t *allocator;
	int rcv_notify_fd;
	union{
		pthread_t tid;
		pid_t pid;
	}id;
	uint8_t work_id;
	llist_t *message_que;
	concurrent_task_admin_t *task_admin;
}concurrent_slave_t;

struct concurrent_message_s{
#define MAX_TASK_KEY_LEN 10
	void *task;
	void *event;
	void (*work_func)(concurrent_slave_t *slave,void *arg);
	uint32_t message_id;
#undef MAX_TASK_KEY_LEN
};

typedef struct concurrent_master_s{
	struct event_base *event_base;
	struct event event;
	uint8_t concurrent_work_type;
	allocator_t *allocator;
	llist_t *message_que;
	llist_t *new_ev_que;
	int *snd_notify_fd;
	concurrent_slave_t *slave;
	uint8_t assignment_count;
	uint8_t slave_amount;
	uint32_t message_count;
	concurrent_task_admin_t *task_admin;
	union{
		pthread_t tid;
		pid_t pid;
	}id;
	int rcv_add_new_event_fd;
	int snd_add_new_event_fd;
	uint8_t concurrent_master_inited_flag;
}concurrent_master_t;

typedef struct concurrent_s{
	struct event new_event;
	uint8_t concurrent_work_type;
	allocator_t *allocator;
	llist_t *new_ev_que;
	union{
		pthread_t tid;
		pid_t pid;
	}id;
	int snd_add_new_event_fd;
	concurrent_master_t *master;
	sync_lock_t concurrent_lock;
}concurrent_t;

concurrent_task_admin_t *concurrent_task_admin_create(allocator_t *allocator);
int concurrent_task_admin_init(concurrent_task_admin_t *task_admin, uint8_t key_size,uint32_t data_size,uint32_t bucket_size, uint8_t admin_lock_type,uint8_t hmap_lock_type);
int concurrent_task_admin_add(concurrent_task_admin_t *task_admin,void *key,void *data);
int concurrent_task_admin_search(concurrent_task_admin_t *task_admin,void *key,hash_map_pos_t *pos);
int concurrent_task_admin_del(concurrent_task_admin_t *task_admin,hash_map_pos_t *pos);
int concurrent_task_admin_destroy(concurrent_task_admin_t *task_admin);
concurrent_master_t *concurrent_master_create(allocator_t *allocator);
void *concurrent_slave_thread(void *arg);
int __concurrent_create_slave(concurrent_master_t *master,uint8_t slave_id);
int concurrent_master_create_slaves(concurrent_master_t *master);
int concurrent_master_destroy_slaves(concurrent_master_t *master);
int concurrent_master_add_message(concurrent_master_t *master,struct concurrent_message_s *message);
int concurrent_master_init_message(struct concurrent_message_s *message, void (*work_func)(concurrent_slave_t *slave,void *arg), void *task, int32_t message_id);
int concurrent_master_destroy(concurrent_master_t *master);
int concurrent_master_init(concurrent_master_t *master, uint8_t concurrent_work_type, uint32_t task_size, uint8_t slave_amount);
void concurrent_master_process_event(int fd, short event, void *arg);
/*
 *int concurrent_master_add_task(concurrent_master_t *master, void *task,void *key, void (*work_func)(concurrent_slave_t *slave,void *arg));
 */
int concurrent_slave_add_new_event(concurrent_slave_t *slave, int fd,int event_flag, struct event *event, void (*event_handler)(int fd, short event, void *arg), void *task);
int concurrent_master_add_new_event(concurrent_master_t *master, int fd,int event_flag, struct event *event, void (*event_handler)(int fd, short event, void *arg), void *arg);
int concurrent_task_admin_del_by_key(concurrent_task_admin_t *task_admin, void *key);
int concurrent_master_choose_slave(concurrent_master_t *master);
int concurrent_master_add_task_and_message(concurrent_master_t *master, void *task,void *key, void (*work_func)(concurrent_slave_t *slave,void *arg));
void *concurrent_master_add_task(concurrent_master_t *master, void *task,void *key);

concurrent_t *concurrent_create(allocator_t *allocator);
int concurrent_init(concurrent_t *c, uint8_t concurrent_work_type, uint32_t task_size, uint8_t slave_amount, uint8_t concurrent_lock_type);
int concurrent_add_event_to_master(concurrent_t *c, int fd,int event_flag, struct event *event, void (*event_handler)(int fd, short event, void *arg), void *arg);
void concurrent_destroy(concurrent_t *c);

#endif
