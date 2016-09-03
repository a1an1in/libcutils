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

client_proxy_t *g_client_proxy;
uint8_t g_proxy_slave_amount = 2;
uint8_t g_proxy_concurrent_lock_type = 0;

client_proxy_t *proxy_get_proxy_addr()
{
	return g_client_proxy;
}
client_proxy_t *proxy_create()
{
	allocator_t *allocator;
	concurrent_t *c ;
	client_proxy_t *proxy;

	if((allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0) ) == NULL){
		dbg_str(DBG_ERROR,"proxy_create allocator_creator err");
		return NULL;
	}
	if ((proxy = (client_proxy_t *)allocator_mem_alloc(allocator, sizeof(client_proxy_t))) == NULL){
		dbg_str(DBG_ERROR,"proxy_create err");
		return NULL;
	}

	c = concurrent_create(allocator);
	proxy->c         = (c != NULL ? c: NULL);
	proxy->allocator = allocator;

	return proxy;
}
int proxy_destroy(client_proxy_t *proxy)
{
	/*
	 *void concurrent_destroy(concurrent_t *c);
	 */
}
int proxy_init(client_proxy_t *proxy, uint32_t task_size, uint8_t slave_amount, uint8_t concurrent_lock_type)
{
	return concurrent_init(proxy->c,
			SERVER_WORK_TYPE_THREAD,
			task_size, 
			slave_amount, 
			concurrent_lock_type);//uint8_t concurrent_lock_type);
}
int proxy_register_client(client_proxy_t *proxy,
		int fd,
		void (*event_handler)(int fd, short event, void *arg),
		struct event *event)
{
	return concurrent_add_event_to_master(proxy->c,
			fd,//int fd,
			EV_READ | EV_PERSIST,//int event_flag,
			event,//struct event *event, 
			event_handler,//void (*event_handler)(int fd, short event, void *arg),
			NULL);//void *arg);
}
int proxy_register_client2(client_proxy_t *proxy,client_t *client)
{
	return concurrent_add_event_to_master2(proxy->c,
			client->client_fd,//int fd,
			EV_READ | EV_PERSIST,//int event_flag,
			&client->event,//struct event *event, 
			client->client_event_handler,//void (*event_handler)(int fd, short event, void *arg),
			client);//void *arg);

}
__attribute__((constructor(111)))
void proxy_constructor()
{
	uint8_t proxy_slave_amount = g_proxy_slave_amount;
	uint8_t proxy_lock_type    = g_proxy_concurrent_lock_type;

	g_client_proxy = proxy_create();
	proxy_init(g_client_proxy, sizeof(client_task_t), proxy_slave_amount, proxy_lock_type);
}
__attribute__((destructor(111)))
void proxy_destructor()
{
	proxy_destroy(g_client_proxy);
}
