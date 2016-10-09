/**
 * @file server.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 1.0
 * @date 2016-03-14
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
#include <errno.h>
#include <string.h>
#include <sys/types.h>     /* basic system data types */
#include <sys/socket.h>    /* basic socket definitions */
#include <netinet/in.h>    /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>     /* inet(3) functions */
#include <sys/epoll.h>     /* epoll function */
#include <fcntl.h>         /* nonblocking */
#include <sys/resource.h>  /*setrlimit */
#include <libconcurrent/concurrent.h>
#include <libipc/net/inet_server.h>
#include <signal.h>


#define MAXEPOLLSIZE 10000
#define MAXLINE 10240

typedef struct iserver_task_s{
	uint32_t fd;
	char key[10];
	struct event *event;
	allocator_t *allocator;
	concurrent_slave_t *slave;
    server_t *server;
}server_task_t;

static int setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
		return -1;
	}
	return 0;

}

#if 0
void master_iserver_listen_event_handler(int fd, short event, void *arg)
{
	concurrent_master_t *master = (concurrent_master_t *)arg;
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t socklen;
	server_task_t *task;
	char key[10];
	struct concurrent_message_s message;

	connfd = accept(fd, (struct sockaddr *)&cliaddr,&socklen);
	if (connfd < 0) {
		perror("accept error");
		return;
	}
	if (setnonblocking(connfd) < 0) {
		perror("setnonblocking error");
	}

	sprintf(key,"%d",connfd);

	dbg_str(NET_DETAIL,"master_iserver_listen_event_handler,listen_fd=%d,connfd=%d",fd,connfd);

	task = (server_task_t *)allocator_mem_alloc(master->allocator,sizeof(server_task_t));
	server_task_init(task,
			connfd,//int fd, 
			key,//void *key, 
			NULL,//struct event *ev,
			master->allocator,
			NULL);

	master->assignment_count++;//do for assigning slave
	concurrent_master_init_message(&message, slave_work_function,task,0);
	concurrent_master_add_message(master,&message);
	dbg_str(NET_DETAIL,"listen end");

    return ;
}
int server(char *host,char *server)
{
	struct addrinfo  *addr, hint;
	int err;
	allocator_t *allocator;
	int ret = 0;
	int server_fd;
	struct event listen_event;
	concurrent_t *c ;

	bzero(&hint, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	if ((err = getaddrinfo(host, server, &hint, &addr)) != 0)
		printf("getaddrinfo error: %s", gai_strerror(err));
	if(addr != NULL){
		dbg_str(NET_DETAIL,"ai_family=%d type=%d",addr->ai_family,addr->ai_socktype);
	}else{
		dbg_str(DBG_ERROR,"getaddrinfo err");
		exit(1);
	}
	server_fd = iserver_create_socket(addr);

	allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0);

	c = concurrent_create(allocator);
	concurrent_init(c,
			SERVER_WORK_TYPE_THREAD,
			sizeof(server_task_t),//uint32_t task_size, 
			2,//uint8_t slave_amount, 
			1);//uint8_t concurrent_lock_type);
	concurrent_add_event_to_master(c,
			server_fd,//int fd,
			EV_READ | EV_PERSIST,//int event_flag,
			&listen_event,//struct event *event, 
			master_iserver_listen_event_handler,//void (*event_handler)(int fd, short event, void *arg),
			NULL);//void *arg);

	freeaddrinfo(addr);

	pause();
	/*
	 *void concurrent_destroy(concurrent_t *c);
	 */

	return ret;
}
#endif
#if 0
/*
 *version 3,master assign task derectly mode,this may have problem,for libevent may not support
 *multi_thread ops to event_base
 */
static void slave_process_conn_bussiness_event_handler(int fd, short event, void *arg)
{
    int nread;
    char buf[MAXLINE];
	server_task_t *task = (server_task_t *)arg;
    nread = read(fd, buf, MAXLINE);//读取客户端socket流

	dbg_str(NET_DETAIL,"slave_work start,conn_fd=%d",fd);
    if (nread < 0) {
        dbg_str(DBG_ERROR,"fd read err,client close the connection");
		server_release_task(task,task->slave->task_admin);
		close(fd);//modify for test
        return;
    } 
    write(fd, buf, nread);//响应客户端  
	server_release_task(task,task->slave->task_admin);
	close(fd);
	dbg_str(NET_DETAIL,"slave_work done");
}

int server_task_init(server_task_t *task,
		int fd, void *key, struct event *ev,
		allocator_t *allocator,
		concurrent_slave_t *slave)
{
	task->fd = fd;
	memcpy(task->key,key,10);
	task->event = ev;
	task->allocator = allocator;
	task->slave = slave;
	return 0;
}
int server_release_task(server_task_t *task,concurrent_task_admin_t *admin)
{
	dbg_str(NET_DETAIL,"lock at release_task");
	sync_lock(&admin->admin_lock,NULL);
	dbg_str(NET_DETAIL,"server_release_task");
	event_del(task->event);
	allocator_mem_free(task->allocator,task->event);
	concurrent_task_admin_del_by_key(admin, task->key);
	sync_unlock(&admin->admin_lock);
	dbg_str(NET_DETAIL,"unlock at release_task");


	return 0;
}
void master_iserver_listen_event_handler(int fd, short event, void *arg)
{
	concurrent_master_t *master = (concurrent_master_t *)arg;
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t socklen;
	server_task_t task,*t;
	hash_map_pos_t pos;
	char key[10];
	concurrent_slave_t *slave;
	struct event *ev;
	int num = 0;

	connfd = accept(fd, (struct sockaddr *)&cliaddr,&socklen);
	if (connfd < 0) {
		perror("accept error");
		return;
	}
	if (setnonblocking(connfd) < 0) {
		perror("setnonblocking error");
	}

	sprintf(key,"%d",connfd);
	num = concurrent_master_choose_slave(master);
	slave = master->slave + num;
	ev = (struct event *)allocator_mem_alloc(master->allocator,sizeof(struct event));
	if(ev == NULL){
		dbg_str(DBG_ERROR,"alloc event err");
		return;
	}

	dbg_str(NET_DETAIL,"master_iserver_listen_event_handler,"
			"slave %d is choosed,listen_fd=%d,connfd=%d",num,fd,connfd);
	server_task_init(&task,
			connfd,//int fd, 
			key,//void *key, 
			ev,//struct event *ev,
			master->allocator,
			slave);

	t = concurrent_master_add_task(master,&task,key);
	concurrent_slave_add_new_event(t->slave,
			t->fd,
			EV_READ | EV_PERSIST,//int event_flag,
			t->event,
			slave_process_conn_bussiness_event_handler,//void (*event_handler)(int fd, short event, void *arg))
			t);

    return ;
}
#endif
#if 0
//version 2, using pipe mode
static void slave_process_conn_bussiness_event_handler(int fd, short event, void *arg)
{
    int nread;
    char buf[MAXLINE];
	server_task_t *task = (server_task_t *)arg;
    nread = read(fd, buf, MAXLINE);//读取客户端socket流

	dbg_str(DBG_VIP,"slave_work start,conn_fd=%d",fd);
    if (nread < 0) {
        dbg_str(DBG_ERROR,"fd read err,client close the connection");
		server_release_task(task,task->slave->task_admin);
		close(fd);//modify for test
        return;
    } 
    write(fd, buf, nread);//响应客户端  
	server_release_task(task,task->slave->task_admin);
	close(fd);
	dbg_str(NET_DETAIL,"slave_work done");
}
int server_task_init(server_task_t *task,
		int fd, void *key, struct event *ev,
		allocator_t *allocator,
		concurrent_slave_t *slave)
{
	task->fd = fd;
	memcpy(task->key,key,10);
	task->event = ev;
	task->allocator = allocator;
	task->slave = slave;
	return 0;
}
int server_release_task(server_task_t *task,concurrent_task_admin_t *admin)
{
	dbg_str(NET_DETAIL,"lock at release_task");
	sync_lock(&admin->admin_lock,NULL);
	dbg_str(NET_DETAIL,"server_release_task");
	event_del(task->event);
	allocator_mem_free(task->allocator,task->event);
	concurrent_task_admin_del_by_key(admin, task->key);
	sync_unlock(&admin->admin_lock);
	dbg_str(NET_DETAIL,"unlock at release_task");


	return 0;
}
void slave_work_function(concurrent_slave_t *slave,void *arg)
{
	server_task_t *task = (server_task_t *)arg;

	dbg_str(NET_DETAIL,"slave_work_function done,rev conn =%d,task key %s",task->fd,task->key);
	task->event = (struct event *)allocator_mem_alloc(slave->allocator,sizeof(struct event));
	task->slave = slave;
	concurrent_slave_add_new_event(
			slave,
			task->fd,//int fd,
			EV_READ | EV_PERSIST,//int event_flag,
			task->event,//	struct event *event,
			slave_process_conn_bussiness_event_handler,//void (*event_handler)(int fd, short event, void *arg),
			task);//void *task);
	return ;
}
void master_iserver_listen_event_handler(int fd, short event, void *arg)
{
	concurrent_master_t *master = (concurrent_master_t *)arg;
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t socklen;
	server_task_t task;
	char key[10];
	struct event *ev;
	int num = 0;

	connfd = accept(fd, (struct sockaddr *)&cliaddr,&socklen);
	if (connfd < 0) {
		perror("accept error");
		return;
	}
	if (setnonblocking(connfd) < 0) {
		perror("setnonblocking error");
	}

	sprintf(key,"%d",connfd);

	dbg_str(NET_DETAIL,"master_iserver_listen_event_handler,"
			"slave %d is choosed,listen_fd=%d,connfd=%d",num,fd,connfd);
	server_task_init(&task,
			connfd,//int fd, 
			key,//void *key, 
			ev,//struct event *ev,
			master->allocator,
			NULL);

	concurrent_master_add_task_and_message(master,&task,(void *)task.key,slave_work_function);
	dbg_str(NET_DETAIL,"listen end");

    return ;
}
#endif
