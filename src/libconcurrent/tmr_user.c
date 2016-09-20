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
#include <libconcurrent/io_user.h>
#include <libconcurrent/tmr_user.h>

tmr_user_t *tmr_user(allocator_t *allocator,
        struct timeval *tv,
        uint16_t timer_flags,
        void (*tmr_event_handler)(int fd, short event, void *arg),
        void *opaque)
{
    concurrent_t *c = concurrent_get_global_concurrent_addr();
	tmr_user_t *tmr_user = NULL;

	if ((tmr_user = (tmr_user_t *)allocator_mem_alloc(
					allocator, sizeof(tmr_user_t))) == NULL)
	{
		dbg_str(DBG_ERROR,"tmr_user_create err");
		return NULL;
	}

	tmr_user->allocator         = allocator;
	tmr_user->tmr_user_fd       = -1;
	tmr_user->opaque            = opaque;
	tmr_user->tmr_event_handler = tmr_event_handler;
	tmr_user->master            = c->master;
    tmr_user->flags             = timer_flags;
    tmr_user->tv                = *tv;

	dbg_str(DBG_DETAIL,"tmr_user->master=%p,allocator=%p",tmr_user->master,allocator);

    concurrent_add_event_to_master(c,
                                   -1,//int fd,
                                   tmr_user->flags,//int event_flag,
                                   &tmr_user->event,//struct event *event, 
                                   &tmr_user->tv,
                                   tmr_user->tmr_event_handler,//void (*event_handler)(int fd, short event, void *arg),
                                   tmr_user);//void *arg);

	return tmr_user;
}
int tmr_user_destroy(tmr_user_t *tmr_user)
{
	allocator_mem_free(tmr_user->allocator,tmr_user);

	return 0;
}


struct timeval lasttime;
int event_is_persistent;
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
int test_tmr_user()
{
	allocator_t *allocator;
    tmr_user_t *timer;
    struct timeval tv;

	if((allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0) ) == NULL){
		dbg_str(DBG_ERROR,"proxy_create allocator_creator err");
		return -1;
	}
	evutil_timerclear(&tv);
	evutil_gettimeofday(&lasttime, NULL);
	tv.tv_sec = 1;
    timer = tmr_user(allocator,
            &tv,
            /*
             *0,
             */
            EV_PERSIST,
            timeout_cb,
            NULL);

#if 0
	evutil_timerclear(&tv);
	tv.tv_sec = 10;
    timer = tmr_user(allocator,
            &tv,
            /*
             *0,
             */
            EV_PERSIST,
            timeout_10s_cb,
            NULL);
#endif
    return 0;
}
