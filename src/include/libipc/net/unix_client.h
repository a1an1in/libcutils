#ifndef __UNIX_SK_CLIENT_H__
#define __UNIX_SK_CLIENT_H__
#include <libipc/net/inet_client.h>
client_t *unix_udp_client(allocator_t *allocator,
                          char *client_unpath,
					      int (*process_task_cb)(client_task_t *task),
					      void *opaque);
int unix_udp_client_send(client_t *client,const void *buf,size_t nbytes,int flags,
		                 const char *dest_unpath);
client_t *tcp_uclient(allocator_t *allocator,
				 	  char *server_unix_path,
				 	  int (*process_task_cb)(client_task_t *task),
				 	  void *opaque);
int tcp_uclient_send(client_t *client,const void *buf,size_t nbytes,int flags);
int uclient_destroy(client_t *client);
#endif



