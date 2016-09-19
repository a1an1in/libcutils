/**
 * @file timer.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 1.0
 * @date 2016-09-19
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

#include <sys/types.h>

#include <event2/event-config.h>

#include <sys/stat.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>

#include <libdbg/debug.h>
#include <libconcurrent/concurrent.h>
#include <libproxy/user.h>
#include <libproxy/proxy.h>
#include <libproxy/timer.h>

struct timeval lasttime;
int event_is_persistent;

ev_timer_t *ev_timer(allocator_t *allocator,
        struct timeval *tv,
        uint16_t timer_flags,
        void (*ev_timer_event_handler)(int fd, short event, void *arg),
        void *opaque)
{
	proxy_t *proxy = proxy_get_proxy_addr();
	ev_timer_t *ev_timer = NULL;

	if ((ev_timer = (ev_timer_t *)allocator_mem_alloc(
					allocator, sizeof(ev_timer_t))) == NULL)
	{
		dbg_str(DBG_ERROR,"ev_timer_create err");
		return NULL;
	}

	ev_timer->allocator              = allocator;
	ev_timer->ev_timer_fd            = -1;
	ev_timer->opaque                 = opaque;
	ev_timer->ev_timer_event_handler = ev_timer_event_handler;
	ev_timer->master                 = proxy->c->master;
    ev_timer->flags                  = timer_flags;
    ev_timer->tv                     = tv;
	dbg_str(DBG_DETAIL,"ev_timer->master=%p,allocator=%p",ev_timer->master,allocator);

	evutil_gettimeofday(&lasttime, NULL);

    proxy_register_timer(proxy,ev_timer);

	return ev_timer;
}
int ev_timer_destroy(ev_timer_t *ev_timer)
{
	allocator_mem_free(ev_timer->allocator,ev_timer);

	return 0;
}

static void
timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	struct timeval newtime, difference;
	struct event *timeout = (struct event *)arg;
	double elapsed;

	evutil_gettimeofday(&newtime, NULL);
	evutil_timersub(&newtime, &lasttime, &difference);
	elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);

	printf("timeout_cb called at %d: %.3f seconds elapsed.\n", (int)newtime.tv_sec, elapsed);
	lasttime = newtime;

}
static void
timeout_10s_cb(evutil_socket_t fd, short event, void *arg)
{
	printf("timeout_10s_cb\n");
}
int test_ev_timer()
{
	allocator_t *allocator;
    ev_timer_t *timer;
    struct timeval tv;

	if((allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0) ) == NULL){
		dbg_str(DBG_ERROR,"proxy_create allocator_creator err");
		return -1;
	}
	evutil_timerclear(&tv);
	tv.tv_sec = 1;
    timer = ev_timer(allocator,
            &tv,
            /*
             *0,
             */
            EV_PERSIST,
            timeout_cb,
            NULL);

	evutil_timerclear(&tv);
	tv.tv_sec = 10;
    timer = ev_timer(allocator,
            &tv,
            /*
             *0,
             */
            EV_PERSIST,
            timeout_10s_cb,
            NULL);
    return 0;
}
