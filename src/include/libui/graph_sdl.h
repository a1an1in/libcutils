#ifndef __GRAPH_SDL_H__
#define __GRAPH_SDL_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/graph.h>
#include <SDL2/SDL.h>

typedef struct sdl_graph_s SDL_Graph;

struct sdl_graph_s{
	Graph graph;

	int (*construct)(SDL_Graph *sdl_graph,char *init_str);
	int (*deconstruct)(SDL_Graph *sdl_graph);
	int (*set)(SDL_Graph *sdl_graph, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*move)(SDL_Graph *sdl_graph);
	int (*init_window)(Graph *graph, void *window);
	int (*close_window)(Graph *graph, void *window);
#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN
	//The window we'll be rendering to
	SDL_Window* window;
	//The surface contained by the window
	SDL_Surface* screen_surface;

};

#endif
