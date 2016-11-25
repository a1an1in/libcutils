#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <libobject/object_deamon.h>
#include <libobject/obj.h>
#include <libjson/cjson.h>

void * object_get_set_func_pointer(void *class_info_addr);
int object_init_func_pointer(void *obj,void *class_info_addr);
class_info_entry_t * object_get_subclass_info(void *class_info_addr);
int object_init(void *obj, char *type_name);
int object_set(void *obj, char *type_name, char *set_str);

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
		((Obj *)obj)->allocator = allocator;\
        dbg_str(DBG_DETAIL,"***********allocator addr:%p",allocator);\
	}\
	\
	obj;\
 })

#define OBJECT_NEW(allocator,type,set_str) \
({\
    void *obj;\
    int ret;\
    obj = OBJECT_ALLOC(allocator,type);\
    ret = object_set(obj, #type, set_str);\
    if( ret < 0) {\
        dbg_str(DBG_ERROR,"object_set error");\
        obj = NULL;\
    } else {\
        object_init(obj,#type);\
    }\
    obj;\
})


#endif
