#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/component.h>
#include <libui/text.h>
#include <libui/color.h>
#include <libui/cursor.h>

typedef struct label_s Label;

struct label_s{
	Component component;

	int (*construct)(Label *label,char *init_str);
	int (*deconstruct)(Label *label);
	int (*set)(Label *label, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*move)(Label *label);
	int (*draw)(Component *component, void *graph);
	int (*load_resources)(Component *component, void *graph);
	int (*unload_resources)(Component *component, void *graph);

#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN
	Text *text;
    String *string;
    cursor_t cursor;
    void *window;
    color_t front_color;
    color_t background_color;
	int char_min_width;
    int char_height;
};

#endif
