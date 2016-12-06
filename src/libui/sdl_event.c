/**
 * @file event_sdl.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-04
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/sdl_event.h>
#include <libui/sdl_window.h>

static int __construct(Event *event,char *init_str)
{
	dbg_str(OBJ_DETAIL,"event construct, event addr:%p",event);

	return 0;
}

static int __deconstrcut(Event *event)
{
	Event *i = (Event *)event;
	dbg_str(OBJ_DETAIL,"event deconstruct,event addr:%p",event);


	return 0;
}

static int __set(Event *event, char *attrib, void *value)
{
    Sdl_Event *e = (Sdl_Event *)event;

	if(strcmp(attrib, "set") == 0) {
		e->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		e->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		e->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		e->deconstruct = value;
	} else if(strcmp(attrib, "poll_event") == 0) {
		e->poll_event = value;
	} else {
		dbg_str(OBJ_WARNNING,"event set,  \"%s\" setting is not support",attrib);
	}

	return 0;
}

static void * __get(Event *event, char *attrib)
{
    if(strcmp(attrib, "") == 0){ 
    } else {
        dbg_str(OBJ_WARNNING,"event get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __poll_event(Event *event)
{
    dbg_str(DBG_DETAIL,"sdl event poll");
    int quit = 0;
	SDL_Event *e = &((Sdl_Event *)event)->ev;
     
    while(!quit) {
         while(SDL_PollEvent(e) != 0) {
             if(e->type == SDL_QUIT) {
                 quit = 1;
             } else {
             }
         }
    }

}

static class_info_entry_t sdl_event_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Event","event",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","poll_event",__poll_event,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Sdl_Event",sdl_event_class_info);

void test_obj_sdl_event()
{
	allocator_t *allocator = allocator_get_default_alloc();
    Window *window;
	Graph *g;
	Event *event;
    char *set_str;
    char buf[2048];

    set_str = (char *)gen_window_setting_str();

    /*
     *window  = OBJECT_NEW(allocator, Sdl_Window,set_str);
     */
    window  = OBJECT_NEW(allocator, Sdl_Window,set_str);
	g       = window->graph;
    event   = window->event;

    object_dump(window, "Sdl_Window", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

    event->poll_event(event);

    object_destroy(window);

    free(set_str);
}


