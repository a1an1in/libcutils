/**
 * @file user.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 1.0
 * @date 2016-09-06
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
#include <libproxy/user.h>
#include <libproxy/proxy.h>


user_t *user(allocator_t *allocator,
			 int user_fd,
			 uint8_t user_type,
			 void (*user_event_handler)(int fd, short event, void *arg),
			 void (*slave_work_function)(concurrent_slave_t *slave,void *arg),
			 int (*process_task_cb)(void *task),
			 void *opaque)
{
	struct addrinfo  *addr, hint;
	int err;
	proxy_t *proxy = proxy_get_proxy_addr();
	user_t *user = NULL;

	if ((user = (user_t *)allocator_mem_alloc(
					allocator, sizeof(user_t))) == NULL)
	{
		dbg_str(DBG_ERROR,"user_create err");
		return NULL;
	}

	user->allocator           = allocator;
	user->user_fd             = user_fd;
	user->user_type           = user_type;
	user->opaque              = opaque;
	user->slave_work_function = slave_work_function;
	user->user_event_handler  = user_event_handler;
	user->master              = proxy->c->master;
	user->process_task_cb     = process_task_cb;
	dbg_str(DBG_DETAIL,"user->master=%p,allocator=%p",user->master,allocator);

	return user;
}
int user_destroy(user_t *user)
{
	allocator_mem_free(user->allocator,user);

	return 0;
}
