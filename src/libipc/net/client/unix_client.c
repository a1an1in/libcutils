/**
 * @file client.c
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
#include <signal.h>
#include <libconcurrent/concurrent.h>
#include <libipc/net/unix_client.h>
#include <sys/un.h>


#define MAXEPOLLSIZE 10000
#define MAXLINE 10240

static int uclient_release_task(client_task_t *task);

static int setnonblocking(int user_fd)
{
	if (fcntl(user_fd, F_SETFL, fcntl(user_fd, F_GETFD, 0)|O_NONBLOCK) == -1) {
		return -1;
	}
	return 0;

}
static void slave_work_function(concurrent_slave_t *slave,void *arg)
{
	client_task_t *task = (client_task_t *)arg;
	client_t *client = task->client;

	dbg_str(NET_DETAIL,"slave_work_function begin");
	client->process_task_cb(task);
	uclient_release_task(task);
	dbg_str(NET_DETAIL,"slave_work_function end");
	return ;
}
#if 1
//version 4, using pipe mode,without task admin
int uclient_init_task(client_task_t *task,
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
	return 0;
}
static int uclient_release_task(client_task_t *task)
{
	allocator_mem_free(task->allocator,task);
	return 0;
}
void uclient_event_handler(int fd, short event, void *arg)
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
	 *dbg_str(NET_DETAIL,"user_fd =%d",fd);
	 */
	nread = read(fd, buf, MAXLINE);//读取客户端socket流
	if (nread < 0) {
		dbg_str(DBG_ERROR,"client_event_handler,read fd error2");
		return;
	} else if(nread == 0){
		dbg_str(DBG_ERROR,"client_event_handler,socket has broken,del client event");
		event_del(&client->event);
		return;
	} else {
		dbg_str(NET_DETAIL,"*************read buf len=%d",nread);
	}

	dbg_str(NET_DETAIL,"client handler allocator=%p",master->allocator);
	task = (client_task_t *)allocator_mem_alloc(master->allocator,sizeof(client_task_t));

	uclient_init_task(task,//client_task_t *task,
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
#endif
static inline 
client_t *__client(allocator_t *allocator,
				   int user_fd,
				   uint8_t socktype,
				   int (*process_task_cb)(client_task_t *task),
				   void *opaque)
{
	return io_user(allocator,//allocator_t *allocator,
			       user_fd,//int user_fd,
			       socktype,//user_type
			       uclient_event_handler,//void (*user_event_handler)(int fd, short event, void *arg),
			       slave_work_function,//void (*slave_work_function)(concurrent_slave_t *slave,void *arg),
			       (int (*)(void *))process_task_cb,//int (*process_task_cb)(user_task_t *task),
			       opaque);//void *opaque)

}
client_t *udp_uclient(allocator_t *allocator,
                     char *client_unpath,
					 int (*process_task_cb)(client_task_t *task),
					 void *opaque)
{
    struct sockaddr_un client_un_addr;
	int err;
	int user_fd;
	client_t *client = NULL;

    if((user_fd = socket(PF_UNIX,SOCK_DGRAM,0)) < 0)  
    {  
        perror("fail to socket");  
        exit(-1);  
    }  

    setnonblocking(user_fd);

    bzero(&client_un_addr,sizeof(client_un_addr));  
    client_un_addr.sun_family = PF_UNIX;  
    strcpy(client_un_addr.sun_path,client_unpath);  
    unlink(client_unpath);

    if(bind(user_fd,(struct sockaddr *)&client_un_addr,sizeof(client_un_addr)) < 0)  
    {  
        perror("fail to bind");  
        exit(-1);  
    }  

	client = __client(allocator,//allocator_t *allocator,
					  user_fd,//int user_fd,
					  SOCK_DGRAM,//uint8_t socktype,
					  process_task_cb,//int (*process_task_cb)(client_task_t *task),
					  opaque);//void *opaque)

	if(client == NULL){
		close(user_fd);
		return NULL;
	}

    strcpy(client->unix_path,client_unpath);

    dbg_str(DBG_DETAIL,"udp_uclient,user_fd=%d",client->user_fd);

	return client;
}
int udp_uclient_send(client_t *client,const void *buf,size_t nbytes,int flags,
		const char *dest_unpath)
{
	int ret = 0;
    struct sockaddr_un dest_un_addr;
    bzero(&dest_un_addr,sizeof(dest_un_addr));  
    dest_un_addr.sun_family = PF_UNIX;  
    strcpy(dest_un_addr.sun_path,dest_unpath);  

	if( ret = sendto(client->user_fd,buf,nbytes,flags,
					 (struct sockaddr *)&dest_un_addr,
					 sizeof(struct sockaddr_un)) < 0)
	{
		perror("sendto()");  
	}  

	return ret;
}

client_t *tcp_uclient(allocator_t *allocator,
				 	  char *server_unix_path,
				 	  int (*process_task_cb)(client_task_t *task),
				 	  void *opaque)
{
	int user_fd;
	struct sockaddr_un sa_addr;
	client_t *client = NULL;
	int ret;
    int len;

    if ((user_fd = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("can't create socket file");
        return NULL;
    }
    setnonblocking(user_fd);

	bzero(&sa_addr, sizeof(sa_addr));
    sa_addr.sun_family = PF_UNIX;  
    strcpy(sa_addr.sun_path,server_unix_path);  
    len = strlen(sa_addr.sun_path) + sizeof(sa_addr.sun_family);

	ret = connect(user_fd,(struct sockaddr *)&sa_addr, len);
	if(ret < 0){
		dbg_str(DBG_ERROR,"connect error,errno=%d",errno);
		close(user_fd);
		return NULL;
	}else{
		dbg_str(NET_DETAIL,"conect suc");
	}

	client = __client(allocator,//allocator_t *allocator,
					  user_fd,//int user_fd,
					  SOCK_STREAM,//uint8_t socktype,
					  process_task_cb,//int (*process_task_cb)(client_task_t *task),
					  opaque);//void *opaque)

	if(client == NULL){
		close(user_fd);
		return NULL;
	}
    memset(client->unix_path,0,sizeof(client->unix_path));
    dbg_str(DBG_DETAIL,"unix_path len=%d",sizeof(client->unix_path));

	return client;
}
int tcp_uclient_send(client_t *client,const void *buf,size_t nbytes,int flags)
{
	int ret = 0;

	if(ret = send(client->user_fd,buf,nbytes,flags) < 0) {
		perror("send()");  
	}  

	return ret;
}
int uclient_destroy(client_t *client)
{
    if(strlen(client->unix_path))
        unlink(client->unix_path);
    else{
        dbg_str(DBG_DETAIL,"unix path is NULL");
    }
    close(client->user_fd);
    io_user_destroy(client);

	return 0;
}
