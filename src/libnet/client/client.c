/**
 * @file client.c
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
#include <signal.h>
#include <libconcurrent/concurrent.h>
#include <libnet/client/client.h>
#include <libnet/client/proxy.h>


#define MAXEPOLLSIZE 10000
#define MAXLINE 10240


int client_release_task(client_task_t *task,concurrent_task_admin_t *admin);

static int setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
		return -1;
	}
	return 0;

}
#if 1
//version 4, using pipe mode,without task admin
int client_release_task_without_task_admin(client_task_t *task);
static void slave_event_handler_process_conn_bussiness(int fd, short event, void *arg)
{
    int nread;
    char buf[MAXLINE];
	client_task_t *task = (client_task_t *)arg;
    nread = read(fd, buf, MAXLINE);//读取客户端socket流

	dbg_str(DBG_VIP,"slave_work start,conn_fd=%d",fd);
    if (nread < 0) {
        dbg_str(DBG_ERROR,"fd read err,client close the connection");
		client_release_task_without_task_admin(task);
		close(fd);//modify for test
        return;
    } 
    write(fd, buf, nread);//响应客户端  
	client_release_task_without_task_admin(task);
	close(fd);
	dbg_str(DBG_DETAIL,"slave_work done");
}
int client_init_task(client_task_t *task,
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
int client_release_task_without_task_admin(client_task_t *task)
{
	event_del(task->event);
	allocator_mem_free(task->allocator,task->event);
	allocator_mem_free(task->allocator,task);

	return 0;
}
static void slave_work_function(concurrent_slave_t *slave,void *arg)
{
	client_task_t *task = (client_task_t *)arg;

	dbg_str(DBG_DETAIL,"slave_work_function begin,rev conn =%d,task key %s",task->fd,task->key);
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
void client_event_handler(int fd, short event, void *arg)
{
	concurrent_master_t *master = (concurrent_master_t *)arg;
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t socklen;
	client_task_t *task;
	char key[10];
	struct concurrent_message_s message;
    int nread;
    char buf[MAXLINE];
	struct sockaddr_in raddr;
	socklen_t raddr_len;

	/*
	 *dbg_str(DBG_DETAIL,"sockfd =%d",fd);
	 */
	nread = read(fd, buf, MAXLINE);//读取客户端socket流
	if (nread < 0) {
		dbg_str(DBG_ERROR,"client_event_handler,read fd error");
		return;
	} 
	/*
	 *if((nread = recvfrom(fd,buf,MAXLINE,0,(void *)&raddr,&raddr_len)) < 0)
	 *{
	 *    perror("recvfrom()");
	 *    exit(1);
	 *}
	 */
	dbg_buf(DBG_DETAIL,"rcv buf:",buf,nread);

#if 0
	task = (client_task_t *)allocator_mem_alloc(master->allocator,sizeof(client_task_t));
	client_init_task(task,
			connfd,//int fd, 
			key,//void *key, 
			NULL,//struct event *ev,
			master->allocator,
			NULL);

	master->assignment_count++;//do for assigning slave
	concurrent_master_init_message(&message, slave_work_function,task,0);
	concurrent_master_add_message(master,&message);
	dbg_str(DBG_DETAIL,"listen end");
#endif

    return ;
}
#endif
int client_create_socket(struct addrinfo *addr)
{
    int listenq = 1024;
    struct rlimit rt;
	int sockfd;
    int opt = 1;

    /* 设置每个进程允许打开的最大文件数 */
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        perror("setrlimit error");
        return -1;
    }

    if ((sockfd = socket(addr->ai_family, addr->ai_socktype, 0)) == -1) {
        perror("can't create socket file");
        return -1;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (setnonblocking(sockfd) < 0) {
        perror("setnonblock error");
    }

    if (bind(sockfd, (struct sockaddr *)addr->ai_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind error");
        return -1;
    } 

    return sockfd;
}
client_t *client(char *host,
				 char *client_port,
				 int family,
				 int socktype,
				 int protocol,
				 void (*event_handler)(int fd, short event, void *arg))
{
	struct addrinfo  *addr, hint;
	int err;
	int client_fd;
	client_proxy_t *proxy = proxy_get_proxy_addr();
	allocator_t *allocator = proxy->allocator;;
	client_t *client, *ret = NULL;

	if ((client = (client_t *)allocator_mem_alloc(
					allocator, sizeof(client_t))) == NULL)
	{
		dbg_str(DBG_ERROR,"client_create err");
		return NULL;
	}

	bzero(&hint, sizeof(hint));
	hint.ai_family   = family;
	hint.ai_socktype = socktype;

	if ((err = getaddrinfo(host, client_port, &hint, &addr)) != 0){
		printf("getaddrinfo error: %s", gai_strerror(err));
		return NULL;
	}
	if(addr != NULL){
		dbg_str(DBG_DETAIL,"ai_family=%d type=%d",addr->ai_family,addr->ai_socktype);
	}else{
		dbg_str(DBG_ERROR,"getaddrinfo err");
		return NULL;
	}
	client->client_fd = client_create_socket(addr);

	if(proxy_register_client(proxy, /*client_proxy_t *proxy*/
				client->client_fd,  /*int fd*/
				event_handler,      /*void (*event_handler)(int fd, short event, void *arg)*/
				&client->event) < 0)/*struct event *event)*/
	{
		goto err_register_client;
	}

	ret = client;
	goto end;

err_register_client:
	close(client->client_fd);
end:
	freeaddrinfo(addr);
	return ret;
}
int test_client()
{
	client( "127.0.0.1",//char *host,
			"1989",//char *client_port,
			AF_INET,//int family,
			SOCK_DGRAM,//int socktype,
			0,//int protocol,
			client_event_handler);//void (*event_handler)(int fd, short event, void *arg));
}
