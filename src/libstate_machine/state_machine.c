/**
 * @file state_machine.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 1.0
 * @date 2016-09-20
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
#include <fcntl.h>              
#include <unistd.h>
#include <libdbg/debug.h>
#include <libdata_structure/vector.h>
#include <libconcurrent/io_user.h>
#include <libconcurrent/tmr_user.h>
#include <libstate_machine/state_machine.h>

state_machine_t *state_machine_create(allocator_t *allocator)
{
    state_machine_t *s;
	int fds[2];

    dbg_str(SM_DETAIL,"state_machine_create");

    if((s = (state_machine_t *)allocator_mem_alloc(allocator,
                    sizeof(state_machine_t))) == NULL)
    {
        dbg_str(SM_ERROR,"allocator_mem_alloc");
        return NULL;
    }
    memset(s,0,sizeof(state_machine_t));

    s->allocator = allocator;

	if((s->vector = vector_create(s->allocator,0)) == NULL){
        dbg_str(SM_ERROR,"vector_create");
        allocator_mem_free(allocator,s);
        return NULL;
    }

    if(pipe(fds)) {
        dbg_str(SM_ERROR,"cannot create pipe");
        allocator_mem_free(allocator,s);
        return NULL;
    }
    s->read_notify_fd  = fds[0];
    s->write_notify_fd = fds[1];

    return s;
}

int state_machine_init(state_machine_t *s,
        void (*notification_event_handler)(int fd, short event, void *arg),
        int entry_num)
{
    dbg_str(SM_DETAIL,"state_machine_init");

	vector_init(s->vector,sizeof(state_entry_t),entry_num);
    s->entry_num = entry_num;

    s->notifier  = io_user(s->allocator,//allocator_t *allocator,
                           s->read_notify_fd,//int user_fd,
                           0,//user_type
                           notification_event_handler,//void (*user_event_handler)(int fd, short event, void *arg),
                           NULL,//void (*slave_work_function)(concurrent_slave_t *slave,void *arg),
                           NULL,//int (*process_task_cb)(user_task_t *task),
                           s);//void *opaque)

    return 0;
}

state_entry_t *init_state_entry(state_entry_t *e,
        state_machine_t *s,
        void (*action_callback)(state_machine_t *s,void *opaque),
        void (*timer_event_callback)(int fd, short event, void *arg),
        time_t       tv_sec,     /* seconds */
        suseconds_t   tv_usec, /* microseconds */
        char *entry_name)
{
    e->timer_event_callback = timer_event_callback;
    e->action_callback  = action_callback;
    e->tv.tv_sec        = tv_sec;
    e->tv.tv_usec       = tv_usec;
    e->state_machine    = s;
    strncpy(e->entry_name,entry_name,strlen(entry_name));

    return e;
}
state_entry_t *state_machine_construct_state_entry(state_machine_t *s,
        void (*action_callback)(state_machine_t *s,void *opaque),
        void (*timer_event_callback)(int fd, short event, void *arg),
        time_t       tv_sec,     /* seconds */
        suseconds_t   tv_usec, /* microseconds */
        char *entry_name)
{
    state_entry_t *e;

    dbg_str(SM_DETAIL,"state_machine_construct_state_entry");
    e = (state_entry_t *)allocator_mem_alloc(s->allocator,sizeof(state_entry_t));
    if(e == NULL){
        dbg_str(SM_ERROR,"allocator_mem_alloc");
        return NULL;
    }
    
    e = init_state_entry(e,
            s,
            action_callback,
            timer_event_callback,
            tv_sec,     /* seconds */
            tv_usec, /* microseconds */
            entry_name);

    state_machine_register_state_entry(s, e);

    return e;
}

void
state_machine_register_state_entry(state_machine_t *s, state_entry_t *e)
{
	vector_push_back(s->vector,e);

    return;
}

int state_machine_setup_entry_timer(state_machine_t *s,uint8_t state)
{
    state_entry_t *e;

    e = (state_entry_t *)vector_get(s->vector,state);

    e->timer = tmr_user(s->allocator,
            &e->tv,
            0,
            e->timer_event_callback,
            (void *)s);
}
int state_machine_stop_entry_timer(state_machine_t *s,uint8_t state)
{
    state_entry_t *e;

    e = (state_entry_t *)vector_get(s->vector,state);

    if(e->timer != NULL)
        tmr_user_destroy(e->timer);

    e->timer = NULL;
}

void
state_machine_change_state(state_machine_t *s, int state)
{
    char command = 'c';//c --> change state
    state_entry_t *e,*le;;

    s->last_state    = s->current_state;
    s->current_state = state;

    e = (state_entry_t *)vector_get(s->vector,s->current_state);
    le = (state_entry_t *)vector_get(s->vector,s->last_state);
    dbg_str(SM_DETAIL," last_state=%d, current_state=%d",s->last_state,s->current_state);
    dbg_str(SM_IMPORTANT,"state_machine_change_state,from %s to %s",le->entry_name,e->entry_name);

	if (write(s->write_notify_fd, &command, 1) != 1) {
		dbg_str(SM_WARNNING,"concurrent_master_notify_slave,write pipe err");
	}
}
static void notification_event_handler(int fd, short event, void *arg)
{
    io_user_t *notifier = (io_user_t *)arg;
    state_machine_t *s = (state_machine_t *)notifier->opaque;
    state_entry_t *e,*le;;

	char buf[1];          

	if (read(s->read_notify_fd, buf, 1) != 1){
		dbg_str(SM_WARNNING,"cannot read form pipe");
		return;
	}
	switch (buf[0]) {
		case 'c': 
            le = (state_entry_t *)vector_get(s->vector,s->last_state);
            state_machine_stop_entry_timer(s,s->last_state);

            e = (state_entry_t *)vector_get(s->vector,s->current_state);
            state_machine_setup_entry_timer(s,s->current_state);
            e->action_callback(s,s->base);
			break;
		case 'b':
			break;            
	}
}

state_machine_t *state_machine(allocator_t *allocator, state_entry_config_t *config)
{
    state_machine_t *s;
    state_entry_t *e;
    int i;

    s = state_machine_create(allocator);

    state_machine_init(s,
            notification_event_handler,//void (*notification_event_handler)(int fd, short event, void *arg),
            10);

    for(i = 0; ; i++){
        if(strlen(config[i].entry_name) != 0){
            e = state_machine_construct_state_entry(s,
                    config[i].action_callback,
                    config[i].timer_event_callback,
                    config[i].tv_sec,     
                    config[i].tv_usec, 
                    config[i].entry_name);
            dbg_str(SM_DETAIL,"config %d, state name :%s",i, config[i].entry_name);
        }else break;
    }

    return s;
}



static void state1_timeout_callback(int fd, short event, void *arg)
{
    dbg_str(SM_DETAIL,"state0_timeout");
}
static void state1_action_callback(state_machine_t *s,void *opaque)
{
    dbg_str(SM_DETAIL,"state0_action");
    state_machine_change_state(s, 2);
}
static void state2_timeout_callback(int fd, short event, void *arg)
{
    tmr_user_t *timer = (tmr_user_t *)arg;
    state_machine_t *s = (state_machine_t *)timer->opaque;
    dbg_str(SM_DETAIL,"state1_timeout");
    state_machine_change_state(s, 4);
}
static void state2_action_callback(state_machine_t *s,void *opaque)
{
    dbg_str(SM_DETAIL,"state1_action");
}
static void state3_timeout_callback(int fd, short event, void *arg)
{
    dbg_str(SM_DETAIL,"state2_timeout");
}
static void state3_action_callback(state_machine_t *s,void *opaque)
{
    dbg_str(SM_DETAIL,"state2_action");
}
static void state4_timeout_callback(int fd, short event, void *arg)
{
    tmr_user_t *timer = (tmr_user_t *)arg;
    state_machine_t *s = (state_machine_t *)timer->opaque;
    dbg_str(SM_DETAIL,"state3_timeout");
    state_machine_change_state(s, 5);
}
static void state4_action_callback(state_machine_t *s,void *opaque)
{
    dbg_str(SM_DETAIL,"state3_action");
}
static void state5_timeout_callback(int fd, short event, void *arg)
{
    tmr_user_t *timer = (tmr_user_t *)arg;
    state_machine_t *s = (state_machine_t *)timer->opaque;
    dbg_str(SM_DETAIL,"state4_timeout");

    state_machine_change_state(s, 2);
}
static void state5_action_callback(state_machine_t *s,void *opaque)
{
    dbg_str(SM_DETAIL,"state4_action");
}

state_entry_config_t entry_config[]={
	{"uninited", NULL,NULL, 1, 0},
	{"init",     state1_action_callback,state1_timeout_callback, 1, 0},
	{"applying", state2_action_callback,state2_timeout_callback, 2, 0},
	{"waiting",  state3_action_callback,state3_timeout_callback, 15, 0},
	{"scanning", state4_action_callback,state4_timeout_callback, 13, 0},
	{"scaned",   state5_action_callback,state5_timeout_callback, 60, 0},
	{"",NULL,NULL,0,0},
};

void test_state_machine()
{
	allocator_t *allocator;
    state_machine_t *s;

    printf("test state_machine\n");

	if((allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC,0) ) == NULL){
		dbg_str(SM_ERROR,"proxy_create allocator_creator err");
		return ;
	}
    s = state_machine(allocator, entry_config);

    state_machine_change_state(s, 1);
}
