#ifndef __CLIENT_PROXY_H__
#define __CLIENT_PROXY_H__

#include <libproxy/user.h>

typedef struct user_proxy_s{
	concurrent_t *c ;
	allocator_t *allocator;
}proxy_t;


proxy_t *proxy_create();
int proxy_destroy(proxy_t *proxy);
int proxy_init(proxy_t *proxy, uint32_t task_size, uint8_t slave_amount, uint8_t concurrent_lock_type);
int proxy_register_user(proxy_t *proxy,user_t *user);
proxy_t *proxy_get_proxy_addr();
/*
 *void proxy_constructor();
 *void proxy_destructor();
 */
#endif
