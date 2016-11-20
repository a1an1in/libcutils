#ifndef __OBJ_H__
#define __OBJ_H__

#include <libobj/obj_deamon.h>

typedef struct obj_s{
	allocator_t *allocator;
	int (*construct)(struct obj_s *obj,char *init_str);
	int (*deconstruct)(struct obj_s *obj);
}obj_t;

#endif
