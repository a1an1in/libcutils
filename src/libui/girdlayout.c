/**
 * @file gridlayout.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/gridlayout.h>
#include <libui/sdl_window.h>
#include <libui/character.h>
#include <libui/timer.h>

static int __construct(Gridlayout *gridlayout,char *init_str)
{
    allocator_t *allocator = ((Obj *)gridlayout)->allocator;

    dbg_str(DBG_DETAIL,"gridlayout construct");

    return 0;
}

static int __deconstrcut(Gridlayout *gridlayout)
{
    dbg_str(DBG_SUC,"gridlayout deconstruct");

    return 0;
}

static int __set(Gridlayout *gridlayout, char *attrib, void *value)
{
    if (strcmp(attrib, "set") == 0) {
        gridlayout->set = value;
    } else if (strcmp(attrib, "get") == 0) {
        gridlayout->get = value;
    } else if (strcmp(attrib, "construct") == 0) {
        gridlayout->construct = value;
    } else if (strcmp(attrib, "deconstruct") == 0) {
        gridlayout->deconstruct = value;
    }
    /*vitual methods*/
    /*attribs*/
    else if (strcmp(attrib, "name") == 0) {
        strncpy(gridlayout->name,value,strlen(value));
    } else {
        dbg_str(DBG_DETAIL,"gridlayout set, not support %s setting",attrib);
    }

    return 0;
}

static void *__get(Gridlayout *obj, char *attrib)
{
    if (strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"gridlayout get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t gridlayout_class_info[] = {
    [0 ] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
    [1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
    [2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
    [3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
    [4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
    [5 ] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
    [6 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Gridlayout",gridlayout_class_info);

char *gen_gridlayout_setting_str()
{
    char *set_str = NULL;

    return set_str;
}

void test_ui_gridlayout()
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

    set_str   = gen_gridlayout_setting_str();
    subject   = OBJECT_NEW(allocator, Gridlayout,set_str);

    object_dump(subject, "Gridlayout", buf, 2048);
    dbg_str(DBG_DETAIL,"Gridlayout dump: %s",buf);

    container->add_component(container,subject);

    dbg_str(DBG_DETAIL,"window container :%p",container);

    window->load_resources(window);
    window->update_window(window);

    event->poll_event(event, window);

    object_destroy(window);
}
