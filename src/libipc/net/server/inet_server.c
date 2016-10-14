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
//version 4, using pipe mode,without task admin
int server_task_init(server_task_t *task,
		             int fd, 
                     void *key, 
                     struct event *ev,
		             allocator_t *allocator,
		             concurrent_slave_t *slave,
                     server_t *server)
{
    memcpy(task->key,key,10);
    task->fd        = fd;
    task->event     = ev;
    task->slave     = slave;
    task->server    = server;
    task->allocator = allocator;
	return 0;
}
int server_task_release_without_task_admin(server_task_t *task)
{
	event_del(task->event);
	allocator_mem_free(task->allocator,task->event);
	allocator_mem_free(task->allocator,task);

	return 0;
}
static void 
slave_process_conn_bussiness_event_handler(int fd, short event, void *arg)
{
	server_task_t *task = (server_task_t *)arg;
	server_t *server    = task->server;

    server->process_task_cb(task);
	server_task_release_without_task_admin(task);
}
static void 
slave_work_function(concurrent_slave_t *slave,void *arg)
{
	server_task_t *task = (server_task_t *)arg;

	dbg_str(NET_DETAIL,"slave_work_function begin,rev conn =%d,task key %s",task->fd,task->key);
	task->event = (struct event *)allocator_mem_alloc(slave->allocator,sizeof(struct event));
	task->slave = slave;

	concurrent_slave_add_new_event(slave,
			                       task->fd,//int fd,
			                       EV_READ | EV_PERSIST,//int event_flag,
			                       task->event,//	struct event *event,
                                   /*void (*event_handler)(int fd, short event, void *arg)*/
			                       slave_process_conn_bussiness_event_handler,
			                       task);//void *task);
	return ;
}
void master_iserver_listen_event_handler(int fd, short event, void *arg)
{
	server_t *server            = (server_t *)arg;
	concurrent_master_t *master = server->master;
	/*
	 *concurrent_master_t *master = (concurrent_master_t *)arg;
	 */
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
			         NULL,
                     server);

	master->assignment_count++;//do for assigning slave
	concurrent_master_init_message(&message, slave_work_function,task,0);
	concurrent_master_add_message(master,&message);
	dbg_str(NET_DETAIL,"listen end");

    return ;
}
int iserver_create_socket(struct addrinfo *addr)
{
    int listenq = 1024;
    struct rlimit rt;
	int listen_fd;
    int opt = 1;

    /* 设置每个进程允许打开的最大文件数 */
	/*
     *rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
     *if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
     *    perror("setrlimit error");
     *    return -1;
     *}
	 */

    if ((listen_fd = socket(addr->ai_family, addr->ai_socktype, 0)) == -1) {
        perror("can't create socket file");
        return -1;
    }

    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (setnonblocking(listen_fd) < 0) {
        perror("setnonblock error");
    }

    if (bind(listen_fd, (struct sockaddr *)addr->ai_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind error");
        return -1;
    } 
    if (listen(listen_fd, listenq) == -1) {
        perror("listen error");
        return -1;
    }

    return listen_fd;
}
server_t * tcp_iserver(allocator_t *allocator,
                       char *host_ip, 
                       char *server_port,
                       int (*process_task_cb)(void *task),
                       void *opaque)
{
	struct addrinfo  *addr, hint;
	int err;
	server_t *srv = NULL;
	int user_fd;

	bzero(&hint, sizeof(hint));
	hint.ai_family   = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	if ((err = getaddrinfo(host_ip, server_port, &hint, &addr)) != 0)
		printf("getaddrinfo error: %s", gai_strerror(err));
	if(addr != NULL){
		dbg_str(NET_DETAIL,"ai_family=%d type=%d",addr->ai_family,addr->ai_socktype);
	}else{
		dbg_str(DBG_ERROR,"getaddrinfo err");
		exit(1);
	}

    user_fd = iserver_create_socket(addr);

	srv = io_user(allocator,//allocator_t *allocator,
			      user_fd,//int user_fd,
			      SOCK_STREAM,//user_type
			      master_iserver_listen_event_handler,//void (*user_event_handler)(int fd, short event, void *arg),
			      slave_work_function,//void (*slave_work_function)(concurrent_slave_t *slave,void *arg),
			      process_task_cb,//int (*process_task_cb)(user_task_t *task),
			      opaque);//void *opaque)

	if(srv == NULL){
		dbg_str(DBG_ERROR,"create srv error");
		return NULL;
	}

	freeaddrinfo(addr);
	return srv;
}
int tcp_iserver_destroy(server_t *server)
{
    close(server->user_fd);
    io_user_destroy(server);

	return 0;
}

static int test_process_task_callback(void *task)
{
    int nread;
    char buf[MAXLINE];
    int fd = ((server_task_t *)task)->fd;

    nread = read(fd, buf, MAXLINE);

	dbg_str(DBG_VIP,"task start,conn_fd=%d",fd);
    if (nread < 0) {
        dbg_str(DBG_ERROR,"fd read err,client close the connection");
		close(fd);//modify for test
        return;
    } 
    write(fd, buf, nread); //ack back 
	/*
	 *close(fd);
	 */
	dbg_str(NET_DETAIL,"task done");
}
int test_iserver()
{
    allocator_t *allocator = allocator_get_default_alloc();

	tcp_iserver(allocator,"127.0.0.1","6888",test_process_task_callback,NULL);

	return 0;
}

