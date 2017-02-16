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

    if ((s = (state_machine_t *)allocator_mem_alloc(allocator,
                                                   sizeof(state_machine_t))) == NULL)
    {
        dbg_str(SM_ERROR,"allocator_mem_alloc");
        return NULL;
    }
    memset(s,0,sizeof(state_machine_t));

    s->allocator = allocator;

    if ((s->vector = vector_create(s->allocator,0)) == NULL){
        dbg_str(SM_ERROR,"vector_create");
        allocator_mem_free(allocator,s);
        return NULL;
    }

    return s;
}

int state_machine_init(state_machine_t *s,
                       int entry_num,
                       void *base)
{
    dbg_str(SM_DETAIL,"state_machine_init");

    vector_init(s->vector,sizeof(state_entry_t),entry_num);
    s->entry_num = entry_num;
    s->base      = base;

    return 0;
}

state_entry_t *init_state_entry(state_entry_t *e,
                                state_machine_t *s,
                                void (*action_callback)(state_machine_t *s,void *opaque),
                                void (*process_timer_task_cb)(void *arg),
                                time_t       tv_sec,     /* seconds */
                                suseconds_t   tv_usec, /* microseconds */
                                char *entry_name)
{
    e->process_timer_task_cb = process_timer_task_cb;
    e->action_callback       = action_callback;
    e->tv.tv_sec             = tv_sec;
    e->tv.tv_usec            = tv_usec;
    e->state_machine         = s;

    memset(e->entry_name, 0,STATE_MACHINE_ENTRY_NAME_MAX_LEN); 
    strncpy(e->entry_name,entry_name,strlen(entry_name));

    return e;
}

state_entry_t *state_machine_construct_state_entry(state_machine_t *s,
        void (*changing_workfunc_cb)(state_machine_t *s,void *opaque),
        void (*process_timer_task_cb)(void *arg),
        time_t tv_sec,     /* seconds */
        suseconds_t tv_usec, /* microseconds */
        char *entry_name)
{
    state_entry_t *e = NULL;

    dbg_str(SM_DETAIL,"state_machine_construct_state_entry");

    e = (state_entry_t *)
        allocator_mem_alloc(s->allocator,sizeof(state_entry_t));

    if (e == NULL){
        dbg_str(SM_ERROR,"allocator_mem_alloc");
        return NULL;
    }
    
    e = init_state_entry(e,
                         s,
                         changing_workfunc_cb,
                         process_timer_task_cb,
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

    dbg_str(SM_DETAIL,"state_machine_setup_entry_timer,tv_sec=%d tv_usec=%d",
            e->tv.tv_sec,e->tv.tv_usec);

    if (e->timer == NULL)
        e->timer = tmr_user(s->allocator,
                            &e->tv,
                            EV_TIMEOUT,
                            e->process_timer_task_cb,
                            (void *)s);
    else
       e->timer = tmr_user_reuse(e->timer);

    return 0;
}

int state_machine_stop_entry_timer(state_machine_t *s,uint8_t state)
{
    state_entry_t *e;

    e = (state_entry_t *)vector_get(s->vector,state);

    dbg_str(SM_DETAIL,"state_machine_stop_entry_timer");
    if (e->timer != NULL){
        dbg_str(SM_DETAIL,"before last event flags=%x,event addr:%p",
                e->timer->event.ev_flags,&e->timer->event);
        /*
         *while(!(e->timer->event.ev_flags & EVLIST_TIMEOUT) && e->timer->event.ev_flags & EVLIST_INIT);
         */
        dbg_str(SM_DETAIL,"last event flags=%x",e->timer->event.ev_flags);
        tmr_user_stop(e->timer);
    } else {
        dbg_str(DBG_WARNNING,"state_machine_stop_entry_timer,timer is NULL");
    }

    /*
     *e->timer = NULL;
     */

    return 0;
}

int state_machine_get_cur_state(state_machine_t *s)
{
    return s->current_state;
}

void
state_machine_change_state(state_machine_t *s, int state)
{
    char command = 'c';//c --> change state
    state_entry_t *e,*le;;

    if (state == s->current_state) return;

    s->last_state    = s->current_state;
    s->current_state = state;

    le = (state_entry_t *)vector_get(s->vector,s->last_state);
    state_machine_stop_entry_timer(s,s->last_state);

    e = (state_entry_t *)vector_get(s->vector,s->current_state);
    state_machine_setup_entry_timer(s,s->current_state);

    dbg_str(SM_SUC,"state_machine_change_state,from %s to %s",le->entry_name,e->entry_name);

    e->action_callback(s,s->base);

}

/**
 * @synopsis    state_machine_change_state_force 
 *              not considering whether last state equal current state
 *
 * @param s
 * @param state
 */
void
state_machine_change_state_force(state_machine_t *s, int state)
{
    char command = 'c';//c --> change state
    state_entry_t *e,*le;;

    s->last_state    = s->current_state;
    s->current_state = state;

    le = (state_entry_t *)vector_get(s->vector,s->last_state);
    e  = (state_entry_t *)vector_get(s->vector,s->current_state);

    dbg_str(SM_SUC,"state_machine_change_state,from %s to %s",le->entry_name,e->entry_name);

    if (write(s->write_notify_fd, &command, 1) != 1) {
        dbg_str(SM_WARNNING,"concurrent_master_notify_slave,write pipe err");
    }
}

state_machine_t *state_machine(allocator_t *allocator, state_entry_config_t *config,void *base)
{
    state_machine_t *s;
    state_entry_t *e;
    int i;

    s = state_machine_create(allocator);

    state_machine_init(s, 10, base);

    for (i = 0; ; i++){
        if (strlen(config[i].entry_name) != 0){
            e = state_machine_construct_state_entry(s,
                                                    config[i].action_callback,
                                                    config[i].process_timer_task_cb,
                                                    config[i].tv_sec,     
                                                    config[i].tv_usec, 
                                                    config[i].entry_name);

            dbg_str(SM_DETAIL,"config %d, state name :%s",i, config[i].entry_name);
        }else break;
    }

    return s;
}

