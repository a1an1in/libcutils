/**
 * @file window.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-03
 */
#include <libui/window.h>
#include <libui/graph_sdl.h>
#include <libui/image_sdl.h>

static int __construct(Window *window,char *init_str)
{
	dbg_str(DBG_SUC,"window construct, window addr:%p",window);
	allocator_t *allocator = ((Obj *)window)->allocator;
	if(window->graph_type == 1) {
		window->graph      = (Graph *)OBJECT_NEW(allocator, SDL_Graph,NULL);
		window->background = (Image *)OBJECT_NEW(allocator, SDL_Image,NULL);
		window->background->path->assign(window->background->path,"./bin/hello_world.bmp");
	}

	if(window->graph != NULL) {
		window->graph->init_window(window->graph,window);
		window->background->load_image(window->background);
	} else {
		dbg_str(DBG_ERROR,"window graph is NULL, please check");
		return -1;
	}

	SDL_Image *i = (SDL_Image *)window->background;
	SDL_Graph *g = (SDL_Graph *)window->graph;
	SDL_BlitSurface(i->surface, NULL, g->screen_surface, NULL );
	SDL_UpdateWindowSurface(g->window);

	return 0;
}

static int __deconstrcut(Window *window)
{
	dbg_str(DBG_SUC,"window deconstruct,window addr:%p",window);
	window->graph->close_window(window->graph,window);

    object_destroy(window->graph);

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

char *gen_window_setting_str()
{
    cjson_t *root,*w, *c, *e, *s;
    char *set_str;

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Window", w = cjson_create_object());{
            cjson_add_item_to_object(root, "Component", c = cjson_create_object());{
                cjson_add_item_to_object(root, "Container", e = cjson_create_object());{
                    cjson_add_item_to_object(e, "Subject", s = cjson_create_object());{
                        cjson_add_number_to_object(s, "x", 1);
                        cjson_add_number_to_object(s, "y", 25);
                        cjson_add_number_to_object(s, "width", 5);
                        cjson_add_number_to_object(s, "height", 125);
                        cjson_add_number_to_object(s, "x_speed", 1.2);
                        cjson_add_number_to_object(s, "y_speed", 2.3);
                    }
                    cjson_add_string_to_object(e, "name", "Container");
                }
                cjson_add_string_to_object(c, "name", "Component");
            }
            cjson_add_string_to_object(w, "name", "Window");
			cjson_add_number_to_object(w, "graph_type", 1);
			cjson_add_number_to_object(w, "screen_width", 640);
			cjson_add_number_to_object(w, "screen_height", 480);
        }
    }
    set_str = cjson_print(root);

    return set_str;
}

void test_ui_window()
{
    Window *window;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    char buf[2048];

    set_str = gen_window_setting_str();

    window = OBJECT_NEW(allocator, Window,set_str);

    object_dump(window, "Window", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

	sleep(5);

    object_destroy(window);

    free(set_str);

}


