#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/component.h>
#include <libui/text.h>
#include <libui/color.h>
#include <libui/cursor.h>

typedef struct girdlayout_s Girdlayout;

struct girdlayout_s{
	Component component;

	int (*construct)(Girdlayout *girdlayout,char *init_str);
	int (*deconstruct)(Girdlayout *girdlayout);
	int (*set)(Girdlayout *girdlayout, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*move)(Girdlayout *girdlayout);
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
    char text_overflow_flag;
};

#endif
