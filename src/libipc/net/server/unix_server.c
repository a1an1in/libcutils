/**
 * @file server.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 1.0
 * @date 2016-09-26
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
#include <libipc/net/unix_server.h>
#include <signal.h>
#include <sys/un.h>

#define MAXEPOLLSIZE 10000

static int setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
		return -1;
	}
	return 0;

}

static int userver_release_task_without_task_admin(server_task_t *task)
{
	event_del(task->event);
	allocator_mem_free(task->allocator,task->event);
	allocator_mem_free(task->allocator,task);

	return 0;
}

static void slave_process_conn_bussiness_event_handler(int fd, short event, void *arg)
{
#define MAX_TASK_BUFFER_LEN 1024
	server_task_t *task = (server_task_t *)arg;
	server_t *server    = task->server;
	server_data_task_t *data_task = NULL;

	data_task = (server_data_task_t *)allocator_mem_alloc(server->allocator,
													      sizeof(server_data_task_t));
	if(data_task == NULL){
		dbg_str(DBG_ERROR,"allocator_mem_alloc,close the connection");
		close(fd);//modify for test
		return;
	}
    data_task->buffer_len = read(task->fd,
								 data_task->buffer,
								 MAX_TASK_BUFFER_LEN);//读取客户端socket流

	dbg_str(DBG_DETAIL,"buffer_len=%d",data_task->buffer_len);
    if (data_task->buffer_len < 0) {
        dbg_str(DBG_ERROR,"fd read err,close the connection");
		close(fd);//modify for test
        return;
	} else if(data_task->buffer_len== 0){
		dbg_str(DBG_ERROR,"client_event_handler,socket has broken,del client event");
		event_del(task->event);
		return;
    } 

	data_task->fd = task->fd;
	data_task->allocator = server->allocator;
	data_task->server = server;

    server->process_task_cb(data_task);

	allocator_mem_free(server->allocator,data_task);

	/*
	 *userver_release_task_without_task_admin(task);
	 */
	//...............fd havn't release
#undef MAX_TASK_BUFFER_LEN
}
static int userver_init_task(server_task_t *task,
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
	task->allocator = allocator;
	task->slave     = slave;
    task->server    = server;
	return 0;
}
static void slave_work_function(concurrent_slave_t *slave,void *arg)
{
	server_task_t *task = (server_task_t *)arg;

	dbg_str(NET_DETAIL,"slave_work_function begin,rev conn =%d,task key %s",
            task->fd,task->key);

	task->event = (struct event *)allocator_mem_alloc(slave->allocator,
                                                      sizeof(struct event));
	task->slave = slave;
	concurrent_slave_add_new_event(slave,
			                       task->fd,//int fd,
			                       EV_READ | EV_PERSIST,//int event_flag,
			                       task->event,//	struct event *event,
			                       slave_process_conn_bussiness_event_handler,
			                       task);//void *task);
	return ;
}
static void unix_tcp_server_listen_event_handler(int fd, short event, void *arg)
{
	server_t *server            = (server_t *)arg;
	concurrent_master_t *master = server->master;
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

	dbg_str(NET_DETAIL,"unix_tcp_server_listen_event_handler,listen_fd=%d,connfd=%d",fd,connfd);

	task = (server_task_t *)
           allocator_mem_alloc(master->allocator,sizeof(server_task_t));

	userver_init_task(task,
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

static int unix_tcp_server_create_socket(char *server_un_path)
{
    int listenq = 1024;
    struct rlimit rt;
	int listen_fd;
    int opt = 1;
    int len;

    struct sockaddr_un server_un_addr;

    if ((listen_fd = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("can't create socket file");
        return -1;
    }

    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (setnonblocking(listen_fd) < 0) {
        perror("setnonblock error");
    }

    unlink(server_un_path); 
    bzero(&server_un_addr,sizeof(server_un_addr));  
    server_un_addr.sun_family = PF_UNIX;  
    strcpy(server_un_addr.sun_path,server_un_path);  
    unlink(server_un_addr.sun_path);
    len = strlen(server_un_addr.sun_path) + sizeof(server_un_addr.sun_family);

    if (bind(listen_fd, (struct sockaddr *)&server_un_addr, len) == -1) {
        perror("bind error");
        return -1;
    } 
    if (listen(listen_fd, listenq) == -1) {
        perror("listen error");
        return -1;
    }

    return listen_fd;
}
server_t * unix_tcp_server(allocator_t *allocator,
                       char *server_un_path,
                       int (*process_task_cb)(void *task),
                       void *opaque)
{
	server_t *srv = NULL;
    int listen_fd;


    listen_fd = unix_tcp_server_create_socket(server_un_path);

	srv = io_user(allocator,//allocator_t *allocator,
			      listen_fd,//int user_fd,
			      SOCK_STREAM,//user_type
			      unix_tcp_server_listen_event_handler,//void (*user_event_handler)(int fd, short event, void *arg),
			      slave_work_function,//void (*slave_work_function)(concurrent_slave_t *slave,void *arg),
			      process_task_cb,//int (*process_task_cb)(user_task_t *task),
			      opaque);//void *opaque)
	if(srv == NULL){
		dbg_str(DBG_ERROR,"create srv error");
		return NULL;
	}
    strcpy(srv->unix_path,server_un_path);

	return srv;
}
int unix_tcp_server_destroy(server_t *server)
{
    unlink(server->unix_path);
    close(server->user_fd);
    io_user_destroy(server);

	return 0;
}

static int test_process_task_callback(void *task)
{
	server_data_task_t *t = (server_data_task_t *)task;;

    write(t->fd, t->buffer,t->buffer_len);//响应客户端  

	return 0;
}
int test_unix_tcp_server()
{
    allocator_t *allocator = allocator_get_default_alloc();

	unix_tcp_server(allocator,"test_server_un_path",test_process_task_callback,NULL);

	return 0;
}
