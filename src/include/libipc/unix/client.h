#ifndef __UNIX_SK_CLIENT_H__
#define __UNIX_SK_CLIENT_H__
#include <libnet/client.h>
client_t *udp_unix_client(allocator_t *allocator,
                     char *client_unpath,
					 int (*process_task_cb)(client_task_t *task),
					 void *opaque);
int udp_unix_client_send(client_t *client,const void *buf,size_t nbytes,int flags,
		const char *dest_unpath);
client_t *tcp_unix_client(allocator_t *allocator,
				 	 char *server_unix_path,
				 	 int (*process_task_cb)(client_task_t *task),
				 	 void *opaque);
int tcp_unix_client_send(client_t *client,const void *buf,size_t nbytes,int flags);
int unix_client_destroy(client_t *client);
#endif



