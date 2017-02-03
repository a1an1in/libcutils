#ifndef __TEXT_AREA_H__
#define __TEXT_AREA_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/string.h>
#include <libui/component.h>
#include <libui/text_wicket.h>

typedef struct text_area_s Text_Area;

struct text_area_s{
	Text_Wicket wicket;

	int (*construct)(Text_Area *text_area,char *init_str);
	int (*deconstruct)(Text_Area *text_area);
	int (*set)(Text_Area *text_area, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/

#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN

    void *window;
};

#endif
