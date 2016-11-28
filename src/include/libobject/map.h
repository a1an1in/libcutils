#ifndef __STRING_H__
#define __STRING_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/obj.h>

typedef struct _map_s Map;

struct _map_s{
	Obj obj;

	int (*construct)(Map *map,char *init_str);
	int (*deconstruct)(Map *map);
	int (*set)(Map *map, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN

};

#endif
