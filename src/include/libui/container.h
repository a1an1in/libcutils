#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/subject.h>
#include <libobject/map.h>

typedef struct container_s Container;

struct container_s{
	Subject subject;

	int (*construct)(Container *container,char *init_str);
	int (*deconstruct)(Container *container);
	int (*set)(Container *container, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*move)(Container *container);
    int (*add_component)(Container *obj, void *component);
    void * (*search_component)(Container *obj, char *key);

#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN
    Map *map;
    char *map_construct_str;
    uint8_t map_type;
};

#endif
