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
#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN

};

#endif
