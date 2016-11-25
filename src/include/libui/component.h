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
#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN

};

#endif
