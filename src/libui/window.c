/**
 * @file window.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-03
 */
#include <libui/window.h>

static int __construct(Window *window,char *init_str)
{
	dbg_str(DBG_SUC,"window construct, window addr:%p",window);

	return 0;
}

static int __deconstrcut(Window *window)
{
	dbg_str(DBG_SUC,"window deconstruct,window addr:%p",window);

	return 0;
}

static int __move(Window *window)
{
	dbg_str(DBG_SUC,"window move");
}

static int __set(Window *window, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		window->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		window->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		window->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		window->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
		window->move = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(window->name,value,strlen(value));
	} else if(strcmp(attrib, "graph_type") == 0) {
        window->graph_type = *((uint8_t *)value);
	} else if(strcmp(attrib, "screen_width") == 0) {
        window->screen_width = *((uint32_t *)value);
	} else if(strcmp(attrib, "screen_height") == 0) {
        window->screen_height = *((uint32_t *)value);
	} else if(strcmp(attrib, "graph") == 0) {
		window->graph = value;
	} else {
		dbg_str(DBG_DETAIL,"window set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Window *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
	} else if(strcmp(attrib, "graph_type") == 0) {
        return &obj->graph_type;
	} else if(strcmp(attrib, "graph") == 0) {
		return obj->graph;
	} else if(strcmp(attrib, "screen_width") == 0) {
		return &obj->screen_width;
	} else if(strcmp(attrib, "screen_height") == 0) {
		return &obj->screen_height;
    } else {
        dbg_str(DBG_WARNNING,"window get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t window_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","move",__move,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[7 ] = {ENTRY_TYPE_UINT8_T,"uint8_t","graph_type",NULL,0},
	[8 ] = {ENTRY_TYPE_UINT32_T,"","screen_width",NULL,sizeof(short)},
	[9 ] = {ENTRY_TYPE_UINT32_T,"","screen_height",NULL,sizeof(short)},
	[10] = {ENTRY_TYPE_NORMAL_POINTER,"Graph","graph",NULL,sizeof(float)},
	[11] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Window",window_class_info);

