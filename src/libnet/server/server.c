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
#include <signal.h>


#define MAXEPOLLSIZE 10000
#define MAXLINE 10240

typedef struct server_task_s{
	uint32_t fd;
	char key[10];
	struct event *event;
	allocator_t *allocator;
	concurrent_slave_t *slave;
}server_task_t;

int server_release_task(server_task_t *task,concurrent_task_admin_t *admin);

int setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
			return -1;
		}
	return 0;
}
static void slave_event_handler_process_conn_bussiness(int fd, short event, void *arg)
{
    int nread;
    char buf[MAXLINE];
	server_task_t *task = (server_task_t *)arg;
    nread = read(fd, buf, MAXLINE);//读取客户端socket流

	dbg_str(DBG_DETAIL,"slave_work start,conn_fd=%d",fd);
    if (nread == 0) {
        printf("client close the connection\n");
        close(fd);
        return;
    } 
    if (nread < 0) {
		/*
         *perror("read error");
		 */
        close(fd);
        return ;
    }    
    write(fd, buf, nread);//响应客户端  
	close(fd);
	server_release_task(task,task->slave->task_admin);
	dbg_str(DBG_DETAIL,"slave_work done");
}

int server_init_task(server_task_t *task,
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
	event_del(task->event);
	allocator_mem_free(task->allocator,task->event);
	concurrent_task_admin_del_by_key(admin, task->key);

	return 0;
}
void master_event_handler_server_listen(int fd, short event, void *arg)
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

	dbg_str(DBG_DETAIL,"master_event_handler_server_listen,"
			"slave %d is choosed,listen_fd=%d,connfd=%d",num,fd,connfd);
	server_init_task(&task,
			connfd,//int fd, 
			key,//void *key, 
			ev,//struct event *ev,
			master->allocator,
			slave);

	concurrent_master_add_task(master,&task,key);

	concurrent_task_admin_search(master->task_admin,key,&pos);
	t = hash_map_pos_get_pointer(&pos);

	concurrent_slave_add_new_event(t->slave,
			t->fd,
			EV_READ | EV_PERSIST,//int event_flag,
			t->event,
			slave_event_handler_process_conn_bussiness,//void (*event_handler)(int fd, short event, void *arg))
			t);
    return ;
}
#if 1
void slave_work_function(concurrent_slave_t *slave,void *arg)
{
	server_task_t *task = (server_task_t *)arg;

	dbg_str(DBG_DETAIL,"slave_work_function done,rev conn =%d,task key %s",task->fd,task->key);
	task->event = (struct event *)allocator_mem_alloc(slave->allocator,sizeof(struct event));
	task->slave = slave;
	concurrent_slave_add_new_event(
			slave,
			task->fd,//int fd,
			EV_READ | EV_PERSIST,//int event_flag,
			task->event,//	struct event *event,
			slave_event_handler_process_conn_bussiness,//void (*event_handler)(int fd, short event, void *arg),
			task);//void *task);
	return ;
}
void master_event_handler_server_listen2(int fd, short event, void *arg)
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

	dbg_str(DBG_DETAIL,"master_event_handler_server_listen,"
			"slave %d is choosed,listen_fd=%d,connfd=%d",num,fd,connfd);
	server_init_task(&task,
			connfd,//int fd, 
			key,//void *key, 
			ev,//struct event *ev,
			master->allocator,
			NULL);

	concurrent_master_add_task_and_message(master,&task,(void *)task.key,slave_work_function);

    return ;
}
#endif
int server_create_socket(struct addrinfo *addr)
{
    int listenq = 1024;
    struct rlimit rt;
	int listen_fd;
    int opt = 1;

    /* 设置每个进程允许打开的最大文件数 */
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        perror("setrlimit error");
        return -1;
    }

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
int server(char *host,char *server)
{
	struct addrinfo  *addr, hint;
	int err;
	allocator_t *allocator;
	concurrent_master_t *master;
	int ret = 0;
	int server_fd;
	struct event listen_event;

	bzero(&hint, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	dbg_str(DBG_DETAIL,"run at here");
	if ((err = getaddrinfo(host, server, &hint, &addr)) != 0)
		printf("getaddrinfo error: %s", gai_strerror(err));
	if(addr != NULL){
		dbg_str(DBG_DETAIL,"ai_family=%d type=%d",addr->ai_family,addr->ai_socktype);
	}else{
		dbg_str(DBG_ERROR,"getaddrinfo err");
		exit(1);
	}
	server_fd = server_create_socket(addr);

	allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0);

	master = concurrent_master_create(allocator);
	concurrent_master_init(master,
			SERVER_WORK_TYPE_THREAD,//uint8_t concurrent_slave_work_function_type,
			sizeof(server_task_t),
			2);

	concurrent_master_add_new_event(master,
			server_fd,
			EV_READ | EV_PERSIST,
			&listen_event,
			master_event_handler_server_listen2,
			NULL);

	freeaddrinfo(addr);

	pause();

	return ret;
}
int test_server()
{
	return server("127.0.0.1","6888");
}
