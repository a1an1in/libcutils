#ifndef __PAINTER_H__
#define __PAINTER_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/obj.h>

typedef struct painter_s Painter;

struct painter_s{
	Obj obj;

	int (*construct)(Painter *painter,char *init_str);
	int (*deconstruct)(Painter *painter);
	int (*set)(Painter *painter, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*move)(Painter *painter);
#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN

};

#endif
