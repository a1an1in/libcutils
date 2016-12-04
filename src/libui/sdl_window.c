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

static int __construct(Window *window,char *init_str)
{
	dbg_str(DBG_SUC,"sdl window construct");
	allocator_t *allocator = ((Obj *)window)->allocator;
	Graph *g;
	Image *background;

	window->graph      = (Graph *)OBJECT_NEW(allocator, SDL_Graph,NULL);
	g = window->graph;

    window->font = OBJECT_NEW(allocator, SDL_Font,"");
	window->font->load_font(window->font);

	window->background = (Image *)OBJECT_NEW(allocator, SDL_Image,NULL);
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
    object_destroy(window->font);
    object_destroy(window->graph);

	return 0;
}

static int __move(Window *window)
{
	dbg_str(DBG_SUC,"window move");
}

static int __set(Window *window, char *attrib, void *value)
{
	SDL_Win *w = (SDL_Win *)window;
	if(strcmp(attrib, "set") == 0) {
		w->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		w->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		w->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		w->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
	} else {
		dbg_str(DBG_DETAIL,"window set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Window *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"window get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t sdl_window_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Window","window",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("SDL_Win",sdl_window_class_info);

static char *gen_window_setting_str()
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

    window  = OBJECT_NEW(allocator, SDL_Win,set_str);
	g       = window->graph;

    object_dump(window, "SDL_Win", buf, 2048);
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


