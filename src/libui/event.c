/**
 * @file event.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-03
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/event.h>

static int __construct(Event *event,char *init_str)
{
	dbg_str(OBJ_DETAIL,"event construct, event addr:%p",event);

	return 0;
}

static int __deconstrcut(Event *event)
{
	dbg_str(OBJ_DETAIL,"event deconstruct,event addr:%p",event);

	return 0;
}

static int __set(Event *event, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		event->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		event->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		event->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		event->deconstruct = value;
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

static class_info_entry_t event_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Event",event_class_info);

void test_obj_event()
{
	Event *event;
	allocator_t *allocator = allocator_get_default_alloc();

    event = OBJECT_NEW(allocator, Event,"");
}


