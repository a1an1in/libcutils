#ifndef __OBJ_DEAMON_H__
#define __OBJ_DEAMON_H__

#include <libdata_structure/map.h>
#include <constructor_priority.h>

typedef struct object_deamon_s{
	allocator_t *allocator;
	map_t *map;
	char map_type;
	uint8_t map_key_len;
	uint8_t map_value_size;
}object_deamon_t;

enum class_info_type_e{
	UNDEFINED = 0, 
	ENTRY_TYPE_INT8_T,
	ENTRY_TYPE_UINT8_T,
	ENTRY_TYPE_INT16_T,
	ENTRY_TYPE_UINT16_T,
	ENTRY_TYPE_INT32_T,
	ENTRY_TYPE_UINT32_T,
	ENTRY_TYPE_INT64_T,
	ENTRY_TYPE_UINT64_T,
	ENTRY_TYPE_FLOAT_T,
	ENTRY_TYPE_OBJ,
	ENTRY_TYPE_NORMAL_POINTER,
	ENTRY_TYPE_FUNC_POINTER,
	ENTRY_TYPE_VIRTUAL_FUNC_POINTER,
	ENTRY_TYPE_OBJ_POINTER,
	ENTRY_TYPE_END,
	ENTRY_TYPE_MAX_TYPE,
};

typedef struct class_info_entry_s{
	uint8_t type;
	char *type_name;
	char *value_name;
	void *value;
	int value_len;
}class_info_entry_t;

extern object_deamon_t *global_object_deamon;

int object_deamon_register_class(object_deamon_t *object_deamon,
							  char *class_name,
							  void *class_info_addr);

void * object_deamon_search_class(object_deamon_t *object_deamon, char *class_name);
object_deamon_t *object_deamon_get_global_object_deamon();
#endif
