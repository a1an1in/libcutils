#ifndef __OBJ_H__
#define __OBJ_H__

#include <libobject/object_deamon.h>

typedef struct obj_s Obj;
struct obj_s{
	allocator_t *allocator;
	int (*construct)(Obj *obj,char *init_str);
	int (*deconstruct)(Obj *obj);
	int (*set)(Obj *obj, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);
};

#endif
