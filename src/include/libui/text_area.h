#ifndef __TEXT_AREA_H__
#define __TEXT_AREA_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/string.h>
#include <libui/component.h>
#include <libui/text.h>
#include <libui/timer.h>
#include <libui/color.h>
#include <libui/cursor.h>

typedef struct text_area_s Text_Area;

typedef struct text_area_line_info_s{
    int paragraph_num;
    int offset;
    int max_height;
}text_area_line_info_t;

struct text_area_s{
	Component component;

	int (*construct)(Text_Area *text_area,char *init_str);
	int (*deconstruct)(Text_Area *text_area);
	int (*set)(Text_Area *text_area, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*draw)(Component *component, void *graph);
	int (*load_resources)(Component *component, void *graph);
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
	int (*load_ascii_info)(Component *component,void *graph);

#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN

    String *string;
	Text *text;
    cursor_t cursor;
    int to_x, to_y, to_max_height;
	int start_line;
	__Timer *timer;
    void *window;
    color_t front_color;
    color_t background_color;
    int cursor_count;
};

#endif
