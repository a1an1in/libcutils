#ifndef __BOX_H__
#define __BOX_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/component.h>
#include <libui/text.h>

typedef struct box_s Box;

struct box_s{
	Component component;

	int (*construct)(Box *box,char *init_str);
	int (*deconstruct)(Box *box);
	int (*set)(Box *box, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*draw)(Component *component, void *graph);
	int (*load_resources)(Component *component, void *graph);
	int (*text_input)(Component *component,char c, void *graph);

#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN
	Text *text;
    int x, y;

};

#endif
