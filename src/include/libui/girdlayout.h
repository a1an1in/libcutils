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

#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN
    void *window;
};

#endif
