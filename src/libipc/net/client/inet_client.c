/**
 * @file client.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 1.0
 * @date 2016-09-03
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
#include <libipc/net/inet_client.h>


#define MAXEPOLLSIZE 10000
#define MAXLINE 1024 * 20

/*
 *extern int process_task_callback(client_task_t *task);
 */

static int iclient_release_task(client_task_t *task);
static int setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
		return -1;
	}
	return 0;

}

static void slave_work_function(concurrent_slave_t *slave,void *arg)
{
	client_task_t *task = (client_task_t *)arg;
	client_t *client = task->client;

	dbg_str(NET_DETAIL,"slave_work_function begin");

	dbg_str(NET_DETAIL,"task_len=%d",task->buffer_len);
	client->process_task_cb(task);
	iclient_release_task(task);
	dbg_str(NET_DETAIL,"slave_work_function end");
	return ;
}

int client_init_task(client_task_t *task,
					 allocator_t *allocator,
					 int fd,
					 struct event *ev,
					 void *key,
					 uint8_t key_len,
					 uint8_t *buf,
					 int buf_len,
					 concurrent_slave_t *slave,
					 client_t *client)
{
	task->fd = fd;
	if(key_len)
		memcpy(task->key,key,key_len);
	if(buf_len)
		memcpy(task->buffer,buf,buf_len);
	task->buffer_len = buf_len;
	task->event      = ev;
	task->allocator  = allocator;
	task->slave      = slave;
	task->client     = client;
    /*
     *dbg_str(DBG_DETAIL,"task len=%d",task->buffer_len);
     */
	return 0;
}

static int iclient_release_task(client_task_t *task)
{
	allocator_mem_free(task->allocator,task);
	return 0;
}

void client_event_handler(int fd, short event, void *arg)
{
	client_t *client = (client_t *)arg;
	concurrent_master_t *master = client->master;
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
	 *dbg_str(NET_DETAIL,"sockfd =%d",fd);
	 */
    dbg_str(NET_DETAIL,"client_event_handler");
	nread = read(fd, buf, MAXLINE);//读取客户端socket流
	if (nread < 0) {
		dbg_str(DBG_ERROR,"client_event_handler,read fd error2");
		return;
	} else if(nread == 0){
		dbg_str(DBG_ERROR,"client_event_handler,socket has broken,del client event");
		event_del(&client->event);
		return;
	} else {
        /*
		 *dbg_str(DBG_DETAIL,"read buf len=%d",nread);
         */
	}
	/*
	 *if((nread = recvfrom(fd,buf,MAXLINE,0,(void *)&raddr,&raddr_len)) < 0)
	 *{
	 *    perror("recvfrom()");
	 *    exit(1);
	 *}
	 */
	/*
	 *dbg_buf(NET_DETAIL,"rcv buf:",buf,nread);
	 */

	dbg_str(NET_DETAIL,"client handler allocator=%p",master->allocator);
	task = (client_task_t *)allocator_mem_alloc(master->allocator,sizeof(client_task_t));

	client_init_task(task,//client_task_t *task,
			    	 master->allocator,//allocator_t *allocator,
			    	 0,//int fd,
			    	 NULL,//struct event *ev,
			    	 NULL,//void *key,
			    	 0,//uint8_t key_len,
			    	 buf,//uint8_t *buf,
			    	 nread,//int buf_len,
			    	 NULL,
			    	 client);

	master->assignment_count++;//do for assigning slave
	concurrent_master_init_message(&message, client->slave_work_function,task,0);
	concurrent_master_add_message(master,&message);
	dbg_str(NET_DETAIL,"client event handler end");

    return ;
}

int inet_udp_client_create_socket(struct addrinfo *addr)
{
    int listenq = 1024;
    struct rlimit rt;
	int sockfd;
    int opt = 1;

    /* 设置每个进程允许打开的最大文件数 */
	/*
     *rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
     *if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
     *    perror("setrlimit error");
     *    return -1;
     *}
	 */

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

static inline 
client_t *__iclient(allocator_t *allocator,
				    int user_fd,
				    uint8_t socktype,
				    int (*process_task_cb)(client_task_t *task),
				    void *opaque)
{
	return io_user(allocator,//allocator_t *allocator,
			       user_fd,//int user_fd,
			       socktype,//user_type
			       client_event_handler,//void (*user_event_handler)(int fd, short event, void *arg),
			       slave_work_function,//void (*slave_work_function)(concurrent_slave_t *slave,void *arg),
			       (int (*)(void *))process_task_cb,//int (*process_task_cb)(user_task_t *task),
			       opaque);//void *opaque)

}

client_t *inet_udp_client(allocator_t *allocator,
					      char *client_ip,
					      char *client_port,
					      int (*process_task_cb)(client_task_t *task),
					      void *opaque)
{
	struct addrinfo  *addr, hint;
	int err;
	int user_fd;
	client_t *client = NULL;

	bzero(&hint, sizeof(hint));
	hint.ai_family   = AF_INET;
	hint.ai_socktype = SOCK_DGRAM;

	if ((err = getaddrinfo(client_ip, client_port, &hint, &addr)) != 0){
		printf("getaddrinfo error: %s", gai_strerror(err));
		return NULL;
	}
	if(addr != NULL){
		dbg_str(NET_DETAIL,"ai_family=%d type=%d",addr->ai_family,addr->ai_socktype);
	}else{
		dbg_str(DBG_ERROR,"getaddrinfo err");
		return NULL;
	}
	user_fd = inet_udp_client_create_socket(addr);

	client = __iclient(allocator,//allocator_t *allocator,
					   user_fd,//int user_fd,
					   SOCK_DGRAM,//uint8_t socktype,
					   process_task_cb,//int (*process_task_cb)(client_task_t *task),
					   opaque);//void *opaque)

	if(client == NULL){
		close(user_fd);
		return NULL;
	}

	return client;
}

int __inet_udp_client_send(client_t *client,
                           const void *buf,
                           size_t nbytes,
                           int flags,
		                   const struct sockaddr *destaddr,
                           socklen_t destlen)
{
	int ret = 0;

	if( ret = sendto(client->user_fd,buf,nbytes,flags,
					 (struct sockaddr *)destaddr,
					 (socklen_t)destlen) < 0)
	{
		perror("sendto()");  
	}  
    /*
     *dbg_buf(DBG_DETAIL,"__inet_udp_client_send:",(uint8_t *)buf,nbytes);
     */

	return ret;
}

int inet_udp_client_send(client_t *client,
                         void *buf,
                         size_t nbytes,
                         int flags,
                         char *dest_id_str, 
                         char *dest_srv_str)
{
    int ret;
    char *type = client->type_str;

    struct sockaddr_in raddr;
    raddr.sin_family = AF_INET; 
    raddr.sin_port = htons(atoi(dest_srv_str));  
    inet_pton(AF_INET,dest_id_str,&raddr.sin_addr);

    ret = __inet_udp_client_send(client,buf,nbytes,flags,
                                 (struct sockaddr *)&raddr,sizeof(raddr));

    return ret;
}
/*
 *int inet_udp_client_broadcast(client_t *cli,char *broadcast_addr, char *dest_port,void *buf,uint32_t len)
 */
int inet_udp_client_broadcast(client_t *cli, char *dest_port,void *buf,uint32_t len)
{
	struct sockaddr_in raddr;

	raddr.sin_family      = AF_INET;
	raddr.sin_port        = htons(atoi(dest_port));
    raddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    int broadcast = 1;
    if( setsockopt(cli->user_fd,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast)) == -1) {
        perror("setsockopt function!\n");
        exit(1);
    }

    __inet_udp_client_send(cli,//client_t *client,
                           buf,//const void *buf,
                           len,
			               0,//int flags,
			               (void *)&raddr,//const struct sockaddr *destaddr,
			               sizeof(raddr));//socklen_t destlen);

    return 0;
}

client_t *inet_tcp_client(allocator_t *allocator,
				 	      char *server_ip,
				 	      char *server_port,
				 	      int (*process_task_cb)(client_task_t *task),
				 	      void *opaque)
{
	int user_fd;
	struct sockaddr_in sa_addr;
	client_t *client = NULL;
	int ret;

	bzero(&sa_addr, sizeof(sa_addr));
	sa_addr.sin_family = AF_INET; 
	sa_addr.sin_port = htons(atoi(server_port));  
	inet_pton(AF_INET,server_ip,&sa_addr.sin_addr);

    if ((user_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("can't create socket file");
        return NULL;
    }
    ret = connect(user_fd,(struct sockaddr *)&sa_addr, sizeof(sa_addr));
	if(ret < 0){
		dbg_str(DBG_ERROR,"connect error,errno=%d",errno);
        perror("connect:");
		close(user_fd);
		return NULL;
	}else{
		dbg_str(NET_DETAIL,"conect suc");
	}

	client = __iclient(allocator,//allocator_t *allocator,
					   user_fd,//int user_fd,
					   SOCK_DGRAM,//uint8_t socktype,
					   process_task_cb,//int (*process_task_cb)(client_task_t *task),
					   opaque);//void *opaque)

	if(client == NULL){
		close(user_fd);
		return NULL;
	}

	return client;
}

int inet_tcp_client_send(client_t *client,const void *buf,size_t nbytes,int flags)
{
	int ret = 0;

	if(ret = send(client->user_fd,buf,nbytes,flags) < 0) {
		perror("send()");  
	}  

	return ret;
}

int inet_client_destroy(client_t *client)
{
    close(client->user_fd);
    io_user_destroy(client);

	return 0;
}
