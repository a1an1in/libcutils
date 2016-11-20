#ifndef __OBJ_STRUCT_H__
#define __OBJ_STRUCT_H__

#include <libobject/object_deamon.h>

typedef struct obj_s{
	allocator_t *allocator;
	int (*construct)(struct obj_s *obj,char *init_str);
	int (*deconstruct)(struct obj_s *obj);
	int (*set)(struct obj_s *obj, char *attrib, void *value);
}obj_t;

#endif
