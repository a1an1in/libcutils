#ifndef __OBJ_H__
#define __OBJ_H__

#include <libobject/object_deamon.h>
#include <libobject/object_struct.h>

void * object_get_set_func_pointer(void *class_info_addr);
int object_init_func_pointer(void *obj,void *class_info_addr);
class_info_entry_t * object_get_subclass_info(void *class_info_addr);
int object_init(void *obj, char *type_name);

#define OBJECT_ALLOC(allocator, type) \
({\
	type * obj; \
	\
	obj = (type *)allocator_mem_alloc(allocator,sizeof(type));\
	if(obj == NULL) {\
		dbg_str(DBG_DETAIL,"allocator_mem_alloc");\
		obj = NULL;\
	} else { \
		memset(obj,0, sizeof(type));\
		((obj_t *)obj)->allocator = allocator;\
	}\
	\
	obj;\
 })

#define OBJECT_NEW(allocator,type,setting_str) \
({\
    void *obj;\
    obj = OBJECT_ALLOC(allocator,type);\
    object_init(obj,#type);\
    obj;\
})


#endif
