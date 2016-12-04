#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/obj.h>
/*
 *#include <libui/window.h>
 */

typedef struct graph_s Graph;

struct graph_s{
	Obj obj;

	int (*construct)(Graph *graph,char *init_str);
	int (*deconstruct)(Graph *graph);
	int (*set)(Graph *graph, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*move)(Graph *graph);
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

};

#endif