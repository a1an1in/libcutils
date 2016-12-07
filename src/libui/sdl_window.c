/**
 * @file window_sdl.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-04
 */
#include <libui/sdl_window.h>
#include <libui/sdl_graph.h>
#include <libui/sdl_image.h>
#include <libui/sdl_font.h>
#include <libui/sdl_event.h>

#if 0
static int __construct(Window *window,char *init_str)
{
	dbg_str(DBG_SUC,"sdl window construct");
	allocator_t *allocator = ((Obj *)window)->allocator;
	Graph *g;
	Image *background;

	window->graph      = (Graph *)OBJECT_NEW(allocator, Sdl_Graph,NULL);
	g = window->graph;

    window->font = OBJECT_NEW(allocator, Sdl_Font,"");
	window->font->load_font(window->font);

    window->event = OBJECT_NEW(allocator, Sdl_Event,"");

	window->background = (Image *)OBJECT_NEW(allocator, Sdl_Image,NULL);
	background = window->background;
	background->path->assign(background->path,"./bin/hello_world.bmp");

	if(g != NULL) {
		g->init_window(g,window);
		/*
		 *background->load_image(background);
		 *g->draw_image(g,background);
		 */
	} else {
		dbg_str(DBG_ERROR,"window graph is NULL, please check");
		return -1;
	}

	g->render_create(g);
	g->render_set_color(g,0xff,0xff,0xff,0xff);
	g->render_clear(g);
	g->render_present(g);
	/*
	 *g->update_window(g);
	 */

	return 0;
}

static int __deconstrcut(Window *window)
{
	Graph *g = window->graph;

	dbg_str(DBG_SUC,"sdl window deconstruct,window addr:%p",window);

	g->render_destroy(g);
	window->graph->close_window(window->graph,window);
    object_destroy(window->background);
    object_destroy(window->event);
    object_destroy(window->font);
    object_destroy(window->graph);

	return 0;
}
#endif 

static int __construct(Window *window,char *init_str)
{
	dbg_str(DBG_SUC,"sdl window construct");

	return 0;
}

static int __deconstrcut(Window *window)
{

	return 0;
}

static int __move(Window *window)
{
	dbg_str(DBG_SUC,"window move");
}

static int __set(Window *window, char *attrib, void *value)
{
	Sdl_Window *w = (Sdl_Window *)window;
	if(strcmp(attrib, "set") == 0) {
		w->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		w->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		w->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		w->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
	} else if(strcmp(attrib, "create_font") == 0) {
		w->create_font = value;
	} else if(strcmp(attrib, "destroy_font") == 0) {
		w->destroy_font = value;
	} else if(strcmp(attrib, "create_graph") == 0) {
		w->create_graph = value;
	} else if(strcmp(attrib, "destroy_graph") == 0) {
		w->destroy_graph = value;
	} else if(strcmp(attrib, "create_event") == 0) {
		w->create_event = value;
	} else if(strcmp(attrib, "destroy_event") == 0) {
		w->destroy_event = value;
	} else if(strcmp(attrib, "create_background") == 0) {
		w->create_background = value;
	} else if(strcmp(attrib, "destroy_background") == 0) {
		w->destroy_background = value;
	} else if(strcmp(attrib, "open_window") == 0) {
		w->open_window = value;
	} else if(strcmp(attrib, "close_window") == 0) {
		w->close_window = value;
	} else {
		dbg_str(DBG_DETAIL,"sdl window set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Window *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"sdl window get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static void *__create_graph(Window *window, char *graph_type)
{
	allocator_t *allocator = ((Obj *)window)->allocator;

    dbg_str(DBG_DETAIL,"sdl window create_graph");

	window->graph = (Graph *)OBJECT_NEW(allocator, Sdl_Graph,NULL);
}

static int __destroy_graph(Window *window)
{
    dbg_str(DBG_DETAIL,"sdl window destroy_font");
    object_destroy(window->graph);
}

static void *__create_font(Window *window,char *font_name)
{
	allocator_t *allocator = ((Obj *)window)->allocator;

    dbg_str(DBG_DETAIL,"sdl window create_font");

    window->font = OBJECT_NEW(allocator, Sdl_Font,"");
	window->font->load_font(window->font);
}

static int __destroy_font(Window *window)
{
    dbg_str(DBG_DETAIL,"sdl window destroy_font");
    object_destroy(window->font);
}

static void *__create_event(Window *window)
{
	allocator_t *allocator = ((Obj *)window)->allocator;

    dbg_str(DBG_DETAIL,"sdl window create_event");

    window->event = OBJECT_NEW(allocator, Sdl_Event,"");
}

static int __destroy_event(Window *window)
{
    dbg_str(DBG_DETAIL,"sdl window destroy_event");
    object_destroy(window->event);
}

static void *__create_background(Window *window, char *pic_path)
{
	allocator_t *allocator = ((Obj *)window)->allocator;
	Image *background;

    dbg_str(DBG_DETAIL,"sdl window draw_background");

	window->background = (Image *)OBJECT_NEW(allocator, Sdl_Image,NULL);
	background = window->background;
	background->path->assign(background->path,"./bin/hello_world.bmp");
}

static int __destroy_background(Window *window)
{
    dbg_str(DBG_DETAIL,"sdl window destroy_background");
    object_destroy(window->background);
}

static int __open_window(Window *window)
{
	Graph *g = window->graph;

    dbg_str(DBG_DETAIL,"sdl window open_window");
	if(g != NULL) {
		g->init_window(g,window);
		/*
		 *background->load_image(background);
		 *g->draw_image(g,background);
		 */
	} else {
		dbg_str(DBG_ERROR,"window graph is NULL, please check");
		return -1;
	}

	g->render_create(g);
	g->render_set_color(g,0xff,0xff,0xff,0xff);
	g->render_clear(g);
	g->render_present(g);
    dbg_str(DBG_DETAIL,"sdl window open_window");
	/*
	 *g->update_window(g);
	 */
}

static int __close_window(Window *window)
{
	Graph *g = window->graph;

    dbg_str(DBG_DETAIL,"sdl window close_window");

	g->render_destroy(g);
	g->close_window(g,window);
}

static class_info_entry_t sdl_window_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Window","window",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","create_graph",__create_graph,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","destroy_graph",__destroy_graph,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","create_font",__create_font,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_FUNC_POINTER,"","destroy_font",__destroy_font,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_FUNC_POINTER,"","create_event",__create_event,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_FUNC_POINTER,"","destroy_event",__destroy_event,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_FUNC_POINTER,"","create_background",__create_background,sizeof(void *)},
	[10] = {ENTRY_TYPE_FUNC_POINTER,"","destroy_background",__destroy_background,sizeof(void *)},
	[11] = {ENTRY_TYPE_FUNC_POINTER,"","open_window",__open_window,sizeof(void *)},
	[12] = {ENTRY_TYPE_FUNC_POINTER,"","close_window",__close_window,sizeof(void *)},
	[13] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Sdl_Window",sdl_window_class_info);

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
                    cjson_add_number_to_object(e, "map_type", 1);
                }
                cjson_add_string_to_object(c, "name", "Component");
            }
            cjson_add_string_to_object(w, "name", "Window");
			cjson_add_number_to_object(w, "screen_width", 640);
			cjson_add_number_to_object(w, "screen_height", 480);
        }
    }
    set_str = cjson_print(root);

    return set_str;
}

void test_ui_sdl_window()
{
    Window *window;
	Graph *g;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    char buf[2048];

    set_str = gen_window_setting_str();

    window  = OBJECT_NEW(allocator, Sdl_Window,set_str);
	g       = window->graph;

    object_dump(window, "Sdl_Window", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

	dbg_str(DBG_DETAIL,"render draw test");
	g->render_load_image(g,window->background);
	g->render_draw_image(g,0,0,window->background);
	g->render_present(g);

	sleep(2);
	g->render_clear(g);
	g->render_set_color(g,0xff,0x0,0xff,0xff);
	g->render_draw_line(g,20,0,50,50);
	g->render_set_color(g,0xff,0x0,0x0,0xff);
	g->render_fill_rect(g,20,20,100,100);
	g->render_present(g);
	sleep(5);

    object_destroy(window);

    free(set_str);

}


