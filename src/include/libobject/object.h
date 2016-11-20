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

/*
 *#define OBJECT_INIT(obj, type_name) \
 *({\
 *    object_deamon_t *deamon;\
 *    void *class_info_addr;\
 * 	class_info_entry_t * subclass_info_addr;\
 *    \
 *    deamon = object_deamon_get_global_object_deamon();\
 *    \
 *    class_info_addr = object_deamon_search_class(deamon,(char *)type_name);\
 *    dbg_str(DBG_DETAIL,"obj_class addr:%p",class_info_addr);\
 *    \
 * 	subclass_info_addr = object_get_subclass_info(class_info_addr);\
 * 	if(subclass_info_addr != NULL) {\
 * 		OBJECT_INIT(obj,subclass_info_addr->type_name);\
 * 	}\
 *    object_init_func_pointer(obj,class_info_addr);\
 *    \
 *    dbg_str(DBG_DETAIL,"obj addr:%p",obj);\
 *    obj->construct(obj,NULL);\
 *    \
 *})
 */

#endif
