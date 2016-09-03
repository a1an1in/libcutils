#ifndef __CLIENT_PROXY_H__
#define __CLIENT_PROXY_H__
typedef struct client_proxy_s{
	concurrent_t *c ;
	allocator_t *allocator;
}client_proxy_t;


client_proxy_t *proxy_create();
int proxy_destroy(client_proxy_t *proxy);
int proxy_init(client_proxy_t *proxy, uint32_t task_size, uint8_t slave_amount, uint8_t concurrent_lock_type);
int proxy_register_client(client_proxy_t *proxy, int fd, void (*event_handler)(int fd, short event, void *arg), struct event *event);
int proxy_register_client2(client_proxy_t *proxy,client_t *client);
client_proxy_t *proxy_get_proxy_addr();
/*
 *void proxy_constructor();
 *void proxy_destructor();
 */
#endif
