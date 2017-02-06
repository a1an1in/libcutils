/**
 * @file girdlayout.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/girdlayout.h>
#include <libui/sdl_window.h>
#include <libui/character.h>
#include <libui/timer.h>

static int __construct(Girdlayout *girdlayout,char *init_str)
{
    allocator_t *allocator = ((Obj *)girdlayout)->allocator;

	dbg_str(DBG_DETAIL,"girdlayout construct");

	return 0;
}

static int __deconstrcut(Girdlayout *girdlayout)
{
	dbg_str(DBG_SUC,"girdlayout deconstruct");

	return 0;
}

static int __set(Girdlayout *girdlayout, char *attrib, void *value)
{
	if (strcmp(attrib, "set") == 0) {
		girdlayout->set = value;
    } else if (strcmp(attrib, "get") == 0) {
		girdlayout->get = value;
	} else if (strcmp(attrib, "construct") == 0) {
		girdlayout->construct = value;
	} else if (strcmp(attrib, "deconstruct") == 0) {
		girdlayout->deconstruct = value;
	}
	/*vitual methods*/
	else if (strcmp(attrib, "draw") == 0) {
		girdlayout->draw = value;
	} else if (strcmp(attrib, "load_resources") == 0) {
		girdlayout->load_resources = value;
	} else if (strcmp(attrib, "unload_resources") == 0) {
		girdlayout->unload_resources = value;
	}
	/*attribs*/
	else if (strcmp(attrib, "name") == 0) {
        strncpy(girdlayout->name,value,strlen(value));
    } else if (strcmp(attrib, "text_overflow_flag") == 0) {
        girdlayout->text_overflow_flag = *((uint8_t *)value);
	} else {
		dbg_str(DBG_DETAIL,"girdlayout set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Girdlayout *obj, char *attrib)
{
    if (strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"girdlayout get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __load_resources(Component *component,void *window)
{
	Graph *g     = ((Window *)window)->graph;
	Girdlayout *girdlayout = (Girdlayout *)component;
	Character *character;

	dbg_str(DBG_DETAIL,"%s load resources",component->name);


	return 0;
}

static int __unload_resources(Component *component,void *window)
{
}

static int __draw(Component *component, void *graph)
{
}

static class_info_entry_t girdlayout_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","load_resources",__load_resources,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_FUNC_POINTER,"","unload_resources",__unload_resources,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_FUNC_POINTER,"","draw",__draw,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[9 ] = {ENTRY_TYPE_INT8_T,"char","text_overflow_flag",NULL,0},
	[10] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Girdlayout",girdlayout_class_info);

char *gen_girdlayout_setting_str()
{
    char *set_str = NULL;

    return set_str;
}

void test_ui_girdlayout()
{
    Window *window;
    Container *container;
	Graph *g;
    Subject *subject;
	__Event *event;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    char buf[2048];

    set_str   = gen_window_setting_str();
    window    = OBJECT_NEW(allocator, Sdl_Window,set_str);
	g         = window->graph;
    event     = window->event;
    container = (Container *)window;

    object_dump(window, "Sdl_Window", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

    set_str   = gen_girdlayout_setting_str();
    subject   = OBJECT_NEW(allocator, Girdlayout,set_str);

    object_dump(subject, "Girdlayout", buf, 2048);
    dbg_str(DBG_DETAIL,"Girdlayout dump: %s",buf);

    container->add_component(container,subject);

	dbg_str(DBG_DETAIL,"window container :%p",container);

	window->load_resources(window);
	window->update_window(window);

    event->poll_event(event, window);

    object_destroy(window);
}
