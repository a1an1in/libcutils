/**
 * @file timer.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-03
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/sdl_timer.h>

extern int SDL_TimerInit(void);
extern void SDL_TimerQuit(void);

static int __construct(__Timer *timer,char *init_str)
{
	dbg_str(OBJ_DETAIL,"timer construct, timer addr:%p",timer);

	return 0;
}

static int __deconstrcut(__Timer *timer)
{
	dbg_str(OBJ_DETAIL,"timer deconstruct,timer addr:%p",timer);

	return 0;
}

static int __set(__Timer *timer, char *attrib, void *value)
{
	Sdl_Timer *t = (Sdl_Timer *)timer;

	if(strcmp(attrib, "set") == 0) {
		t->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		t->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		t->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		t->deconstruct = value;
	} else if(strcmp(attrib, "set_timer") == 0) {
		t->set_timer = value;
	} else if(strcmp(attrib, "reuse") == 0) {
		t->reuse = value;
	} else {
		dbg_str(OBJ_WARNNING,"timer set,  \"%s\" setting is not support",attrib);
	}

	return 0;
}

static void * __get(__Timer *timer, char *attrib)
{
    if(strcmp(attrib, "") == 0){ 
    } else {
        dbg_str(OBJ_WARNNING,"timer get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __set_timer(__Timer *timer,uint32_t time, void *callback)
{
	Sdl_Timer *st = (Sdl_Timer *)timer;
	uint32_t (*cb)(uint32_t interval, void* param);

	cb = callback;
	st->interval = time;
	st->callback = callback;
	st->timer_id = SDL_AddTimer(time, cb, timer);
}

static int __reuse(__Timer *timer)
{
	Sdl_Timer *st = (Sdl_Timer *)timer;

	st->timer_id = SDL_AddTimer(st->interval, st->callback, st);
}
static class_info_entry_t sdl_timer_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"__Timer","timer",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","set_timer",__set_timer,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_FUNC_POINTER,"","reuse",__reuse,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Sdl_Timer",sdl_timer_class_info);

static uint32_t test_sdl_timer_callback(uint32_t interval, void* param )
{
	Sdl_Timer *timer = (Sdl_Timer *)param;

	SDL_RemoveTimer(timer->timer_id);

	timer->timer_id = timer->set_timer((__Timer *)timer,
									   timer->interval,
									   timer->callback);
}
void test_obj_sdl_timer()
{
	__Timer *timer;
	allocator_t *allocator = allocator_get_default_alloc();

	if( SDL_Init(SDL_INIT_TIMER ) < 0 ) {
		dbg_str(DBG_ERROR,"SDL_Init");
	}
    timer = OBJECT_NEW(allocator, Sdl_Timer,"");
	timer->set_timer(timer, 3 * 1000, test_sdl_timer_callback);
	pause();
}


