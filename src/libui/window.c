/**
 * @file window.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-03
 */
#include <libui/window.h>
#include <miscellany/buffer.h>

typedef struct position_s{
	int x;
	int y;
}position_t;

static int __construct(Window *window,char *init_str)
{
	dbg_str(DBG_DETAIL,"window construct, window addr:%p",window);
    window->create_graph(window, NULL);
    window->create_font(window,NULL);
    window->create_event(window);
    window->open_window(window);

    window->create_background(window, "./bin/hello_world.bmp");//must create after openning window

	return 0;
}

static int __deconstrcut(Window *window)
{
	dbg_str(DBG_DETAIL,"window deconstruct,window addr:%p",window);
    window->destroy_background(window);
    window->close_window(window);
    window->destroy_event(window);
    window->destroy_font(window);
    window->destroy_graph(window);

	return 0;
}

static int __set(Window *window, char *attrib, void *value)
{
	/*normal methods*/
	if(strcmp(attrib, "set") == 0) {
		window->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		window->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		window->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		window->deconstruct = value;
	} else if(strcmp(attrib, "update_window") == 0) {
		window->update_window = value;
	} else if(strcmp(attrib, "load_resources") == 0) {
		window->load_resources = value;
	}
	/*vitual methods*/
	else if(strcmp(attrib, "move") == 0) { 
		window->move = value;
	} else if(strcmp(attrib, "create_font") == 0) {
		window->create_font = value;
	} else if(strcmp(attrib, "destroy_font") == 0) {
		window->destroy_font = value;
	} else if(strcmp(attrib, "create_graph") == 0) {
		window->create_graph = value;
	} else if(strcmp(attrib, "destroy_graph") == 0) {
		window->destroy_graph = value;
	} else if(strcmp(attrib, "create_event") == 0) {
		window->create_event = value;
	} else if(strcmp(attrib, "destroy_event") == 0) {
		window->destroy_event = value;
	} else if(strcmp(attrib, "create_background") == 0) {
		window->create_background = value;
	} else if(strcmp(attrib, "destroy_background") == 0) {
		window->destroy_background = value;
	} else if(strcmp(attrib, "init_window") == 0) {
		window->init_window = value;
	} else if(strcmp(attrib, "open_window") == 0) {
		window->open_window = value;
	} else if(strcmp(attrib, "close_window") == 0) {
		window->close_window = value;
	} else if(strcmp(attrib, "create_timer") == 0) {
		window->create_timer = value;
	} else if(strcmp(attrib, "remove_timer") == 0) {
		window->remove_timer = value;
	} else if(strcmp(attrib, "destroy_timer") == 0) {
		window->destroy_timer = value;
	}
	/*attribs*/
	else if(strcmp(attrib, "name") == 0) { 
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

static void window_draw_component(Iterator *iter, void *arg) 
{
	Component *component;
	uint8_t *addr;
	Graph *g = (Graph *)arg;

	dbg_str(DBG_DETAIL,"window_draw_component");
	addr = (uint8_t *)iter->get_vpointer(iter);
	component = (Component *)buffer_to_addr(addr);
	component->draw(component, g);
}

static int __update_window(Window *window)
{
	Graph *g = window->graph;
    Container *container = (Container *)window;

    dbg_str(DBG_DETAIL,"window update_window");
	container->for_each_component(container, window_draw_component, g);
	g->render_present(g);

}

static void window_load_component_resources(Iterator *iter, void *arg) 
{
	Component *component;
	Window *window = (Window *)arg;
	uint8_t *addr;

	dbg_str(DBG_DETAIL,"window_load_component_resources");
	addr = (uint8_t *)iter->get_vpointer(iter);
	component = (Component *)buffer_to_addr(addr);
	component->load_resources(component, window);
}

static int __load_resources(Window *window)
{
	Container *container = (Container *)window;

	dbg_str(DBG_DETAIL,"window load_resources");
	container->for_each_component(container, window_load_component_resources, window);

	return 0;
}

#if 0
static int __open_window(Window *window)
{
	Graph *g = window->graph;

    dbg_str(DBG_DETAIL,"sdl window open_window start");

	if(g == NULL) {
		dbg_str(DBG_ERROR,"window graph is NULL, please check");
		return -1;
	} else {
		/*
		 *background->load_image(background);
		 *g->draw_image(g,background);
		 */
	}

    window->init_window(window);

    g->set_window(g, window);
	g->render_create(g);
	g->render_set_color(g,0xff,0xff,0xff,0xff);
	g->render_clear(g);
	g->render_present(g);

    dbg_str(DBG_DETAIL,"sdl window open_window end");
	/*
	 *g->update_window(g);
	 */
}
#endif


static class_info_entry_t window_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","load_resources",__load_resources,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_FUNC_POINTER,"","update_window",__update_window,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_VFUNC_POINTER,"","move",NULL,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_VFUNC_POINTER,"","create_font",NULL,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_VFUNC_POINTER,"","destroy_font",NULL,sizeof(void *)},
	[10] = {ENTRY_TYPE_VFUNC_POINTER,"","create_graph",NULL,sizeof(void *)},
	[11] = {ENTRY_TYPE_VFUNC_POINTER,"","destroy_graph",NULL,sizeof(void *)},
	[12] = {ENTRY_TYPE_VFUNC_POINTER,"","create_event",NULL,sizeof(void *)},
	[13] = {ENTRY_TYPE_VFUNC_POINTER,"","destroy_event",NULL,sizeof(void *)},
	[14] = {ENTRY_TYPE_VFUNC_POINTER,"","create_background",NULL,sizeof(void *)},
	[15] = {ENTRY_TYPE_VFUNC_POINTER,"","destroy_background",NULL,sizeof(void *)},
	[16] = {ENTRY_TYPE_VFUNC_POINTER,"","init_window",NULL,sizeof(void *)},
	[17] = {ENTRY_TYPE_VFUNC_POINTER,"","open_window",NULL,sizeof(void *)},
	[18] = {ENTRY_TYPE_VFUNC_POINTER,"","close_window",NULL,sizeof(void *)},
	[19] = {ENTRY_TYPE_VFUNC_POINTER,"","create_timer",NULL,sizeof(void *)},
	[20] = {ENTRY_TYPE_VFUNC_POINTER,"","remove_timer",NULL,sizeof(void *)},
	[21] = {ENTRY_TYPE_VFUNC_POINTER,"","destroy_timer",NULL,sizeof(void *)},
	[22] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[23] = {ENTRY_TYPE_UINT8_T,"uint8_t","graph_type",NULL,0},
	[24] = {ENTRY_TYPE_UINT32_T,"","screen_width",NULL,sizeof(short)},
	[25] = {ENTRY_TYPE_UINT32_T,"","screen_height",NULL,sizeof(short)},
	[26] = {ENTRY_TYPE_NORMAL_POINTER,"Graph","graph",NULL,sizeof(float)},
	[27] = {ENTRY_TYPE_END},
};
REGISTER_CLASS("Window",window_class_info);

