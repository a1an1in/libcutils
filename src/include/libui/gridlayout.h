#ifndef __GRIDLAYOUT_H__
#define __GRIDLAYOUT_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/component.h>
#include <libui/text.h>
#include <libui/color.h>
#include <libui/cursor.h>

typedef struct gridlayout_s Gridlayout;

struct gridlayout_s{
	Component component;

	int (*construct)(Gridlayout *gridlayout,char *init_str);
	int (*deconstruct)(Gridlayout *gridlayout);
	int (*set)(Gridlayout *gridlayout, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
    int (*add_component)(Container *obj, void *component);
	int (*load_resources)(Component *component, void *window);
	int (*draw)(Component *component, void *graph);

#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN
    void *window;
    uint32_t *col_width, *row_height; 
    uint32_t row_max, col_max, cur_row, cur_col;
    uint32_t default_unit_width, default_unit_height;

    Component **grid_components;
};

#endif
