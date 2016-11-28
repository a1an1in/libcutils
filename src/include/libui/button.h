#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/component.h>

typedef struct button_s Button;

struct button_s{
	Component component;

	int (*construct)(Button *button,char *init_str);
	int (*deconstruct)(Button *button);
	int (*set)(Button *button, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*move)(Button *button);
#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN

};

#endif