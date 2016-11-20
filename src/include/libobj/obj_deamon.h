#ifndef __OBJ_DEAMON_H__
#define __OBJ_DEAMON_H__

#include <libdata_structure/map.h>
#include <constructor_priority.h>

typedef struct obj_deamon_s{
	allocator_t *allocator;
	map_t *map;
	char map_type;
	uint8_t map_key_len;
	uint8_t map_value_size;
}obj_deamon_t;

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

extern obj_deamon_t *global_obj_deamon;

int obj_deamon_register_class(obj_deamon_t *obj_deamon,
							  char *class_name,
							  void *class_info_addr);

void * obj_deamon_search_class(obj_deamon_t *obj_deamon, char *class_name);
obj_deamon_t *obj_deamon_get_global_obj_deamon();
#endif
