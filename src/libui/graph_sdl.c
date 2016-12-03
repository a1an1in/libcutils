/**
 * @file sdl_grath.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/graph_sdl.h>
#include <libui/window.h>

static int __construct(SDL_Graph *sdl_grath,char *init_str)
{
	dbg_str(DBG_SUC,"sdl_grath construct, sdl_grath addr:%p",sdl_grath);

	return 0;
}

static int __deconstrcut(SDL_Graph *sdl_grath)
{
	dbg_str(DBG_SUC,"sdl_grath deconstruct,sdl_grath addr:%p",sdl_grath);

	return 0;
}

static int __set(SDL_Graph *sdl_grath, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		sdl_grath->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		sdl_grath->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		sdl_grath->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		sdl_grath->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
		sdl_grath->move = value;
	} else if(strcmp(attrib, "init_window") == 0) {
		sdl_grath->init_window = value;
	} else if(strcmp(attrib, "close_window") == 0) {
		sdl_grath->close_window = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(sdl_grath->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"sdl_graph set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(SDL_Graph *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"sdl_grath get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __init_window(SDL_Graph *graph, void *window)
{
	int ret;
	Window *w = (Window *)window;

	dbg_str(DBG_SUC,"SDL_Graph create window");

	dbg_str(DBG_DETAIL,"srceen width=%d, height=%d",w->screen_width,w->screen_height);
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		dbg_str(DBG_ERROR,"SDL could not initialize! SDL_Error: %s", SDL_GetError() );
		ret = -1;
	} else {
		//Create window
		graph->window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w->screen_width, w->screen_height, SDL_WINDOW_SHOWN );
		if( graph->window == NULL ) {
			dbg_str(DBG_ERROR,"Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			ret = -1;
		} else {
			graph->screen_surface = SDL_GetWindowSurface(graph->window);
			ret = 1;
		}
	}

	return ret;
}

static int __close_window(SDL_Graph *graph, void *window)
{
	Window *w = (Window *)window;
	dbg_str(DBG_SUC,"SDL_Graph close_window");

	//release screen surface
	SDL_FreeSurface( graph->screen_surface );

	//Destroy window
	SDL_DestroyWindow(graph->window);
	graph->window = NULL;
	
	//Quit SDL subsystems
	SDL_Quit();
}
static class_info_entry_t sdl_grath_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Graph","graph",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_FUNC_POINTER,"","init_window",__init_window,sizeof(void *)},
	[6] = {ENTRY_TYPE_FUNC_POINTER,"","close_window",__close_window,sizeof(void *)},
	[7] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[8] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("SDL_Graph",sdl_grath_class_info);

void test_ui_sdl_grath()
{
    SDL_Graph *sdl_grath;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    cjson_t *root, *e, *s;
    char buf[2048];

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "SDL_Graph", e = cjson_create_object());{
            cjson_add_string_to_object(e, "name", "alan");
        }
    }

    set_str = cjson_print(root);

    sdl_grath = OBJECT_NEW(allocator, SDL_Graph,set_str);

    object_dump(sdl_grath, "SDL_Graph", buf, 2048);
    dbg_str(DBG_DETAIL,"SDL_Graph dump: %s",buf);

    object_destroy(sdl_grath);

    free(set_str);

}


