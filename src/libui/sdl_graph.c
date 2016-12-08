/**
 * @file sdl_grath.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/window.h>
#include <libui/sdl_graph.h>
#include <libui/sdl_image.h>
#include <libui/sdl_text.h>

static int __construct(Sdl_Graph *sdl_grath,char *init_str)
{
	dbg_str(DBG_DETAIL,"sdl_grath construct, sdl_grath addr:%p",sdl_grath);

	return 0;
}

static int __deconstrcut(Sdl_Graph *sdl_grath)
{
	dbg_str(DBG_DETAIL,"sdl_grath deconstruct,sdl_grath addr:%p",sdl_grath);

	return 0;
}

static int __set(Sdl_Graph *sdl_grath, char *attrib, void *value)
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
	} else if(strcmp(attrib, "update_window") == 0) {
		sdl_grath->update_window = value;
	} else if(strcmp(attrib, "draw_image") == 0) {
		sdl_grath->draw_image = value;
	} else if(strcmp(attrib, "render_create") == 0) {
		sdl_grath->render_create = value;
	} else if(strcmp(attrib, "render_destroy") == 0) {
		sdl_grath->render_destroy = value;
	} else if(strcmp(attrib, "render_set_color") == 0) {
		sdl_grath->render_set_color = value;
	} else if(strcmp(attrib, "render_clear") == 0) {
		sdl_grath->render_clear = value;
	} else if(strcmp(attrib, "render_draw_line") == 0) {
		sdl_grath->render_draw_line = value;
	} else if(strcmp(attrib, "render_fill_rect") == 0) {
		sdl_grath->render_fill_rect = value;
	} else if(strcmp(attrib, "render_draw_image") == 0) {
		sdl_grath->render_draw_image = value;
	} else if(strcmp(attrib, "render_load_image") == 0) {
		sdl_grath->render_load_image = value;
	} else if(strcmp(attrib, "render_load_text") == 0) {
		sdl_grath->render_load_text = value;
	} else if(strcmp(attrib, "render_write_text") == 0) {
		sdl_grath->render_write_text = value;
	} else if(strcmp(attrib, "render_present") == 0) {
		sdl_grath->render_present = value;
	} else if(strcmp(attrib, "name") == 0) { /**attribs*/
        strncpy(sdl_grath->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"sdl_graph set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Sdl_Graph *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"sdl_grath get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __init_window(Sdl_Graph *graph, void *window)
{
	int ret;
	Window *w = (Window *)window;

	dbg_str(DBG_DETAIL,"Sdl_Graph create window");

	dbg_str(DBG_DETAIL,"srceen width=%d, height=%d",w->screen_width,w->screen_height);
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		dbg_str(DBG_ERROR,"SDL could not initialize! SDL_Error: %s", SDL_GetError() );
		ret = -1;
	} else {
		//Create window
		graph->window = SDL_CreateWindow("SDL Tutorial", 
                                         SDL_WINDOWPOS_UNDEFINED, 
                                         SDL_WINDOWPOS_UNDEFINED,
                                         w->screen_width, 
                                         w->screen_height,
                                         SDL_WINDOW_SHOWN );
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

static int __close_window(Sdl_Graph *graph, void *window)
{
	Window *w = (Window *)window;
	dbg_str(DBG_DETAIL,"Sdl_Graph close_window");

	//release screen surface
	SDL_FreeSurface( graph->screen_surface );

	//Destroy window
	SDL_DestroyWindow(graph->window);
	graph->window = NULL;
	
	//Quit SDL subsystems
	SDL_Quit();
}

static int __update_window(Sdl_Graph *graph)
{
	dbg_str(DBG_DETAIL,"Sdl_Graph update_window");
	SDL_UpdateWindowSurface(graph->window);
}

static int __draw_image(Sdl_Graph *graph, void *image)
{
	Sdl_Image *i = (Sdl_Image *)image;
	dbg_str(DBG_DETAIL,"Sdl_Graph draw_image");

	SDL_BlitSurface(i->surface, NULL, graph->screen_surface, NULL );
}

static int __render_create(Sdl_Graph *graph)
{
	dbg_str(DBG_DETAIL,"Sdl_Graph render_create");
	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
	graph->render = SDL_CreateRenderer( graph->window, -1, 0 );
}

static int __render_destroy(Sdl_Graph *graph)
{
	dbg_str(DBG_DETAIL,"Sdl_Graph render_destroy");
	SDL_DestroyRenderer(graph->render);
}

static int __render_set_color(Sdl_Graph *graph,
							  uint8_t r, uint8_t g,
							  uint8_t b, uint8_t a)
{
	dbg_str(DBG_DETAIL,"Sdl_Graph set_color");
	SDL_SetRenderDrawColor(graph->render, r, g, b, a);
}

static int __render_clear(Sdl_Graph *graph)
{
	dbg_str(DBG_DETAIL,"Sdl_Graph render_clear");
	SDL_RenderClear(graph->render);
}

static int __render_draw_line(Sdl_Graph *graph,int x1, int y1, int x2, int y2)
{
	dbg_str(DBG_DETAIL,"Graph render_draw_line");
	SDL_RenderDrawLine(graph->render,x1, y1, x2, y2);
}

static int __render_fill_rect(Sdl_Graph *graph,int x1, int y1, int x2, int y2)
{
	dbg_str(DBG_DETAIL,"Graph render_fill_rect");
	SDL_Rect fillRect = {x1,y1,x2,y2};
	SDL_RenderFillRect(graph->render, &fillRect );
}

static int __render_load_image(Sdl_Graph *graph,void *image)
{
	Sdl_Image *i = (Sdl_Image *)image;
	dbg_str(DBG_DETAIL,"SDL Graph render_load_image");
	if(i->surface == NULL) {
		i->load_image(image);
	}

	if(i->surface != NULL) {
		i->texture = SDL_CreateTextureFromSurface(graph->render, i->surface);
		i->width = i->surface->w;
		i->height = i->surface->h;
		SDL_FreeSurface(i->surface);
	}
}

static int __render_draw_image(Sdl_Graph *graph,int x, int y, void *image)
{
	dbg_str(DBG_DETAIL,"Sdl_Graph render_draw_image");
	Sdl_Image *i = (Sdl_Image *)image;
	SDL_Rect render_quad = { x, y, i->width, i->height};
	SDL_RenderCopy(graph->render, i->texture, NULL, &render_quad );
}

static int __render_load_text(Sdl_Graph *graph,void *text,void *font,int r, int g, int b, int a)
{

	Sdl_Text *t = (Sdl_Text *)text;
	Sdl_Font *f = (Sdl_Font *)font;
	SDL_Surface* surface = NULL;
	SDL_Color textColor = {r, g, b, a };

	dbg_str(DBG_DETAIL,"Sdl_Text load text");
	surface = TTF_RenderText_Solid(f->ttf_font,
                                   ((Text *)text)->content->value,
                                   textColor ); 

	if(surface != NULL) {
		t->texture = SDL_CreateTextureFromSurface(graph->render, surface);
		t->width = surface->w;
		t->height = surface->h;
		dbg_str(DBG_DETAIL,"width =%d height=%d",t->width, t->height);
		SDL_FreeSurface(surface);
	}

}
static int __render_write_text(Sdl_Graph *graph,int x, int y, void *text)
{
	dbg_str(DBG_DETAIL,"Sdl_Graph render_write_text");
	Sdl_Text *t = (Sdl_Text *)text;
	SDL_Rect render_quad = { x, y, t->width, t->height};
	SDL_RenderCopy(graph->render, t->texture, NULL, &render_quad );
}
static int __render_present(Sdl_Graph *graph)
{
	dbg_str(DBG_DETAIL,"Sdl_Graph render_present");
	SDL_RenderPresent(graph->render);
}

static class_info_entry_t sdl_grath_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Graph","graph",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","init_window",__init_window,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_FUNC_POINTER,"","close_window",__close_window,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_FUNC_POINTER,"","update_window",__update_window,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_FUNC_POINTER,"","draw_image",__draw_image,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_FUNC_POINTER,"","render_create",__render_create,sizeof(void *)},
	[10] = {ENTRY_TYPE_FUNC_POINTER,"","render_destroy",__render_destroy,sizeof(void *)},
	[11] = {ENTRY_TYPE_FUNC_POINTER,"","render_set_color",__render_set_color,sizeof(void *)},
	[12] = {ENTRY_TYPE_FUNC_POINTER,"","render_clear",__render_clear,sizeof(void *)},
	[13] = {ENTRY_TYPE_FUNC_POINTER,"","render_draw_line",__render_draw_line,sizeof(void *)},
	[14] = {ENTRY_TYPE_FUNC_POINTER,"","render_fill_rect",__render_fill_rect,sizeof(void *)},
	[15] = {ENTRY_TYPE_FUNC_POINTER,"","render_draw_image",__render_draw_image,sizeof(void *)},
	[16] = {ENTRY_TYPE_FUNC_POINTER,"","render_load_image",__render_load_image,sizeof(void *)},
	[17] = {ENTRY_TYPE_FUNC_POINTER,"","render_load_text",__render_load_text,sizeof(void *)},
	[18] = {ENTRY_TYPE_FUNC_POINTER,"","render_write_text",__render_write_text,sizeof(void *)},
	[19] = {ENTRY_TYPE_FUNC_POINTER,"","render_present",__render_present,sizeof(void *)},
	[20] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[21] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Sdl_Graph",sdl_grath_class_info);

void test_ui_sdl_grath()
{
    Sdl_Graph *sdl_grath;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    cjson_t *root, *e, *s;
    char buf[2048];

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Sdl_Graph", e = cjson_create_object());{
            cjson_add_string_to_object(e, "name", "alan");
        }
    }

    set_str = cjson_print(root);

    sdl_grath = OBJECT_NEW(allocator, Sdl_Graph,set_str);

    object_dump(sdl_grath, "Sdl_Graph", buf, 2048);
    dbg_str(DBG_DETAIL,"Sdl_Graph dump: %s",buf);

    object_destroy(sdl_grath);

    free(set_str);

}


