#ifndef __GRAPH_SDL_H__
#define __GRAPH_SDL_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/graph.h>
#include <SDL2/SDL.h>

typedef struct sdl_graph_s Sdl_Graph;

struct sdl_graph_s{
	Graph graph;

	int (*construct)(Sdl_Graph *sdl_graph,char *init_str);
	int (*deconstruct)(Sdl_Graph *sdl_graph);
	int (*set)(Sdl_Graph *sdl_graph, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*move)(Sdl_Graph *sdl_graph);
	int (*init_window)(Graph *graph, void *window);
	int (*close_window)(Graph *graph, void *window);
	int (*update_window)(Graph *graph);
	int (*draw_image)(Graph *graph, void *image);
	int (*render_create)(Graph *graph);
	int (*render_destroy)(Graph *graph);
	int (*render_set_color)(Graph *graph, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	int (*render_clear)(Graph *graph);
	int (*render_draw_line)(Graph *graph,int x1, int y1, int x2, int y2);
	int (*render_fill_rect)(Graph *graph,int x1, int y1, int x2, int y2);
	int (*render_load_image)(Graph *graph,void *image);
	int (*render_draw_image)(Graph *graph,int x, int y, void *image);
	int (*render_load_text)(Graph *graph,void *text,void *font,int r, int g, int b, int a);
	int (*render_write_text)(Graph *graph,int x, int y, void *text);
	int (*render_present)(Graph *graph);

#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN
	//The window we'll be rendering to
	SDL_Window* window;
	//The surface contained by the window
	SDL_Surface* screen_surface;
	SDL_Renderer* render;

};

#endif
