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
	Event *i = (Event *)event;

	if(strcmp(attrib, "set") == 0) {
		i->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		i->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		i->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		i->deconstruct = value;
	} else {
		dbg_str(OBJ_WARNNING,"event set,  \"%s\" setting is not support",attrib);
	}

	return 0;
}

static void * __get(Event *event, char *attrib)
{
    if(strcmp(attrib, "x") == 0){ 
    } else {
        dbg_str(OBJ_WARNNING,"event get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}


static class_info_entry_t sdl_event_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Event","event",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("_SDL_Event",sdl_event_class_info);

void test_obj_sdl_event()
{
    Window *window;
	Graph *g;
	Event *event;
	Font *font;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    char buf[2048];

    set_str = gen_window_setting_str();

    window  = OBJECT_NEW(allocator, SDL_Win,set_str);
	g       = window->graph;

    object_dump(window, "SDL_Win", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

	/*
     *font = OBJECT_NEW(allocator, SDL_Font,"");
	 *font->load_font(font);
	 */


	pause();

    object_destroy(window);

    free(set_str);

}


