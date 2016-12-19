#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/container.h>

typedef struct component_s Component;

struct component_s{
	Container container;

	int (*construct)(Component *component,char *init_str);
	int (*deconstruct)(Component *component);
	int (*set)(Component *component, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*move)(Component *component);
	int (*draw)(Component *component, void *graph);
	int (*load_resources)(Component *component, void *window);
	int (*text_key_input)(Component *component,char c, void *graph);
	int (*backspace_key_input)(Component *component,void *graph);
	int (*up_key_down)(Component *component,void *graph);
	int (*down_key_down)(Component *component,void *graph);
	int (*left_key_down)(Component *component,void *graph);
	int (*right_key_down)(Component *component,void *graph);
	int (*pageup_key_down)(Component *component,void *graph);
	int (*pagedown_key_down)(Component *component,void *graph);
	int (*one_line_up)(Component *component,void *graph);
	int (*one_line_down)(Component *component,void *graph);

#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN

};

#endif
