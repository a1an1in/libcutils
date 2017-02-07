/**
 * @file timer.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-18
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/timer.h>

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
    if(strcmp(attrib, "set") == 0) {
        timer->set = value;
    } else if(strcmp(attrib, "get") == 0) {
        timer->get = value;
    } else if(strcmp(attrib, "construct") == 0) {
        timer->construct = value;
    } else if(strcmp(attrib, "deconstruct") == 0) {
        timer->deconstruct = value;
    } else if(strcmp(attrib, "set_timer") == 0) {
        timer->set_timer = value;
    } else if(strcmp(attrib, "reuse") == 0) {
        timer->reuse = value;
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

static class_info_entry_t timer_class_info[] = {
    [0 ] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
    [1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
    [2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
    [3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
    [4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
    [5 ] = {ENTRY_TYPE_VFUNC_POINTER,"","set_timer",NULL,sizeof(void *)},
    [6 ] = {ENTRY_TYPE_VFUNC_POINTER,"","reuse",NULL,sizeof(void *)},
    [7 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("__Timer",timer_class_info);

void test_obj_timer()
{
    __Timer *timer;
    allocator_t *allocator = allocator_get_default_alloc();

    timer = OBJECT_NEW(allocator, __Timer,"");
}


