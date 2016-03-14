/**
 * @file concurrent.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-03-11
 */

/* Copyright (c) 2015-2020 alan lin <a1an1in@sina.com>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <libdbg/debug.h>
#include <libconcurrent/concurrent.h>

concurrent_task_admin_t *
concurrent_task_admin_create(allocator_t *allocator)
{
	concurrent_task_admin_t *task_admin = NULL;

	task_admin = (concurrent_task_admin_t *)allocator_mem_alloc(allocator,sizeof(concurrent_task_admin_t));
	if(task_admin == NULL){
		dbg_str(DBG_ERROR,"allocc concurrent task admin err");
		exit(1);
	}
	task_admin->allocator = allocator;

	return task_admin;
}
int concurrent_task_admin_init(concurrent_task_admin_t *task_admin,
		uint8_t key_size,uint32_t data_size,uint32_t bucket_size,
		uint8_t admin_lock_type,uint8_t hmap_lock_type)
{
	int ret = 0;

	task_admin->admin_lock_type = admin_lock_type;
	task_admin->hmap_lock_type = hmap_lock_type;
	task_admin->key_size = key_size;
	task_admin->data_size = data_size;
	task_admin->bucket_size = bucket_size;

	/*
	 *task_admin->hmap = hash_map_create(allocator,PTHREAD_MUTEX_LOCK);
	 */
	task_admin->hmap = hash_map_create(task_admin->allocator,hmap_lock_type);
	hash_map_init(task_admin->hmap,
			key_size,//uint32_t key_size,
			key_size + data_size,
			bucket_size,
			NULL,
			NULL);
	//if concurrent work mode is process,then the lock need be modified!!!!
	sync_lock_init(&task_admin->admin_lock,admin_lock_type);

	return ret;
}
int concurrent_task_admin_add(concurrent_task_admin_t *task_admin,void *key,void *data)
{
	pair_t *pair;

	//malloc problem ,need modify
	
	pair = create_pair(task_admin->key_size,task_admin->data_size);
	make_pair(pair,key,data);
	hash_map_insert(task_admin->hmap,pair->data);

	return 0;
}
int concurrent_task_admin_search(concurrent_task_admin_t *task_admin,
		void *key,hash_map_pos_t *pos)
{
	return hash_map_search(task_admin->hmap,key,pos);
}
int concurrent_task_admin_del(concurrent_task_admin_t *task_admin,
		hash_map_pos_t *pos)
{
	return hash_map_delete(task_admin->hmap,pos);
}
int concurrent_task_admin_destroy(concurrent_task_admin_t *task_admin)
{
	hash_map_destroy(task_admin->hmap);
	sync_lock_destroy(&task_admin->admin_lock);
	allocator_mem_free(task_admin->allocator,task_admin);

	return 0;
}

static void slave_event_handler_process_message(int fd, short event, void *arg)
{
	concurrent_slave_t *slave = (concurrent_slave_t *)arg;
	char buf[1];          
	list_t *l;
	struct concurrent_message_s *message;

	dbg_str(DBG_VIP,"slave_event_handler_process_message,fd=%d",fd);
	if (read(fd, buf, 1) != 1){
		dbg_str(DBG_WARNNING,"cannot read form pipe");
		return;
	}
	switch (buf[0]) {
		case 'c': 
			l = llist_detach_front(slave->message_que);
			message = (struct concurrent_message_s *)l->data;
			message->work_func(slave,message->task);
			allocator_mem_free(slave->allocator,l);
			break;
		case 'p':
			break;            
	}

}
void *concurrent_slave_thread(void *arg)
{
	concurrent_slave_t *slave = (concurrent_slave_t *)arg;

	dbg_str(DBG_DETAIL,"concurrent_slave_thread start,concurrent_slave_thread id=%d",slave->work_id);

	slave->event_base = event_init();
	if(slave->event_base == NULL){
		dbg_str(DBG_ERROR,"cannot create slave event_base");
		exit(1);
	}
	event_set(&slave->message_event,slave->rcv_notify_fd,
	              EV_READ | EV_PERSIST, slave_event_handler_process_message, arg);
	event_base_set(slave->event_base, &slave->message_event);
	if (event_add(&slave->message_event, 0) == -1) {
		dbg_str(DBG_WARNNING,"event_add err");
	}
	event_base_loop(slave->event_base, 0);
	return NULL;
}
int concurrent_slave_add_new_event(concurrent_slave_t *slave,
		int fd,int event_flag,
		void (*event_handler)(int fd, short event, void *arg),
		struct event *event)
{
	event_set(event,fd, event_flag, event_handler, slave);
	event_base_set(slave->event_base, event);
	if (event_add(event, 0) == -1) {
		dbg_str(DBG_WARNNING,"event_add err");
	}
	return 0;
}
int __concurrent_master_create_slave(concurrent_master_t *master,uint8_t slave_id)
{
	int ret = 0;
	concurrent_slave_t *slave = &master->slave[slave_id];
	int fds[2];

	slave->message_que = master->message_que;
	slave->allocator = master->allocator;
	slave->task_admin = master->task_admin;

	if(master->concurrent_work_type == SERVER_WORK_TYPE_THREAD) {

		slave->work_id = slave_id;
		if(pipe(fds)) {
			dbg_str(DBG_ERROR,"cannot create pipe");
			exit(1);
		}
		slave->rcv_notify_fd = fds[0];
		master->snd_notify_fd[slave_id] = fds[1];

		if ((ret = pthread_create(&slave->id.tid, NULL, concurrent_slave_thread, slave)) != 0) {
			dbg_str(DBG_ERROR,"cannot slave pthread");
			exit(1);
		}

	} else if ( master->concurrent_work_type == SERVER_WORK_TYPE_PROCESS) {
	
	} else {
		dbg_str(DBG_ERROR,"not support concurrent_work_type");
		return -1;
	} 

	return ret;
}
int concurrent_master_create_slaves(concurrent_master_t *master)
{
	int i = 0, ret = 0;

	master->slave = (concurrent_slave_t *)allocator_mem_alloc(master->allocator,
			sizeof(concurrent_slave_t) * master->slave_amount);
	if(master->slave == NULL){
		dbg_str(DBG_ERROR,"alloc slave err");
		ret = -1;
		goto end;
	}
	master->snd_notify_fd = (int *)allocator_mem_alloc(master->allocator,
			sizeof(int) * master->slave_amount);
	if(master->slave == NULL){
		dbg_str(DBG_ERROR,"alloc snd_notify_fd err");
		ret = -1;
		goto end;
	}

	for (i = 0; i < master->slave_amount; i++) {
		ret = __concurrent_master_create_slave(master, i);
	}

end:
	return ret;

}
int concurrent_master_destroy_slaves(concurrent_master_t *master)
{
}



concurrent_master_t *concurrent_master_create(allocator_t *allocator)
{
	concurrent_master_t *master;

	master = (struct concurrent_master_s *)allocator_mem_alloc(allocator,
			sizeof(struct concurrent_master_s));
	master->allocator = allocator;
	master->event_base = event_init();
	master->concurrent_master_inited_flag = 0;

	return master;
}
/**
 * @synopsis master_event_handler_add_new_event 
 * 			to add new event,which master thread is listening 
 *
 * @param fd 
 * @param event
 * @param arg
 */
static void master_event_handler_add_new_event(
		int fd, short event, void *arg)
{
	concurrent_master_t *master  = (concurrent_master_t *)arg;
	char buf[1];          

	if (read(fd, buf, 1) != 1){
		dbg_str(DBG_WARNNING,"cannot read form pipe");
		return;
	}
	switch (buf[0]) {
		case 'c': 
			if (event_add(&master->event, 0) == -1) {
				dbg_str(DBG_WARNNING,"event_add err");
			}
			break;
		case 'p':
			break;            
	}

}
void *concurrent_master_thread(void *arg)
{
	concurrent_master_t *master  = (concurrent_master_t *)arg;
	struct event event;

	dbg_str(DBG_DETAIL,"concurrent_master_thread start");

	master->event_base = event_init();
	/*
	 *listenning rcv_add_new_event_fd, we can designe any right fd here,
	 *we just need event base become loop state.
	 */
	event_set(&event,master->rcv_add_new_event_fd, EV_READ | EV_PERSIST,
			master_event_handler_add_new_event, master);
	event_base_set(master->event_base, &event);
	if (event_add(&event, 0) == -1) {
		dbg_str(DBG_WARNNING,"event_add err");
	}

	dbg_str(DBG_DETAIL,"concurrent_master_thread end");
	master->concurrent_master_inited_flag = 1;
	event_base_loop(master->event_base, 0);

	return NULL;
}
int concurrent_master_init(concurrent_master_t *master,
		uint8_t concurrent_work_type,
		uint32_t task_size,
		uint8_t slave_amount)
{
	int ret = 0;
	int fds[2];

	master->concurrent_work_type = concurrent_work_type;
	master->slave_amount = slave_amount;
	master->message_que = llist_create(master->allocator,1);
	master->message_count = 0;
	llist_init(master->message_que,sizeof(struct concurrent_message_s));

	// create task admin
	master->task_admin = concurrent_task_admin_create(master->allocator);
	ret = concurrent_task_admin_init(master->task_admin,
				4,
				task_size,
				10,//uint32_t bucket_size,
				0,//uint8_t admin_lock_type,
				1);//uint8_t hmap_lock_type)

	if(pipe(fds)) {
		dbg_str(DBG_ERROR,"cannot create pipe");
		exit(1);
	}

	master->rcv_add_new_event_fd = fds[0];
	master->snd_add_new_event_fd = fds[1];

	if ((ret = pthread_create(&master->id.tid, NULL, concurrent_master_thread, master)) != 0) {
		dbg_str(DBG_ERROR,"cannot slave pthread");
		exit(1);
	}

	concurrent_master_create_slaves(master);
	return ret;
}
int concurrent_master_add_task(concurrent_master_t *master,
		void *task,void *key,
		void (*work_func)(concurrent_slave_t *slave,void *arg))
{
	struct concurrent_message_s message;
	hash_map_pos_t pos;
	void *t;

	concurrent_task_admin_add(master->task_admin,key,task);
	concurrent_task_admin_search(master->task_admin,key,&pos);

	t = hash_map_pos_get_pointer(&pos);
	concurrent_master_init_message(&message, work_func,t,0);
	concurrent_master_add_message(master,&message);

	return 0;
}
static void concurrent_master_notify_slave(concurrent_master_t *master,char command)
{
	int i = 0;

	i = master->message_count % master->slave_amount;
	dbg_str(DBG_DETAIL,"concurrent_master_notify_slave,slave i=%d is assigned,notify fd=%d",i,master->snd_notify_fd[i]);

	if (write(master->snd_notify_fd[i], &command, 1) != 1) {
		dbg_str(DBG_WARNNING,"concurrent_master_notify_slave,write pipe err");
	}
}
int concurrent_master_add_message(concurrent_master_t *master,
		struct concurrent_message_s *message)
{
	char command = 'c';

	llist_push_back(master->message_que,message);
	master->message_count++;
	concurrent_master_notify_slave(master,command);

	return 0;
}
int concurrent_master_init_message(struct concurrent_message_s *message,
		void (*work_func)(concurrent_slave_t *slave,void *arg),
		void *task,
		int32_t message_id)
{
	message->work_func = work_func;
	message->task = task;
	message->message_id = message_id;

	return 0;
}
/**
 * @synopsis concurrent_master_add_new_event 
 * 			 we can assign task or listenning fd to slave thread through this func
 *
 * @param master
 * @param fd
 * @param event_flag
 * @param event_handler
 * @param event
 *
 * @returns   
 */
int concurrent_master_add_new_event(concurrent_master_t *master,
		int fd,int event_flag,
		void (*event_handler)(int fd, short event, void *arg),
		struct event *event)
{
	while(master->concurrent_master_inited_flag == 0);	

	dbg_str(DBG_DETAIL,"init new event");
	event_set(event,fd, event_flag, event_handler, master);
	event_base_set(master->event_base, event);

	dbg_str(DBG_DETAIL,"add new event");
	if (event_add(event, 0) == -1) {
		dbg_str(DBG_WARNNING,"event_add err");
	}
	/*
	 *if (write(master->snd_add_new_event_fd, "c", 1) != 1) {
	 *    dbg_str(DBG_ERROR,"cannot write pipe");
	 *}
	 */
	return 0;
}
int concurrent_master_destroy(concurrent_master_t *master)
{
	concurrent_task_admin_destroy(master->task_admin);
	concurrent_master_destroy_slaves(master);
	llist_destroy(master->message_que);
	//... release event base??
	allocator_mem_free(master->allocator,master);
	return 0;
}
