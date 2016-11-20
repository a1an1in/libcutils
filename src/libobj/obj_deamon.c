#include <stdio.h>
#include <libdbg/debug.h>
#include <libobj/obj_deamon.h>
#include <miscellany/buffer.h>

obj_deamon_t *global_obj_deamon;

obj_deamon_t * obj_deamon_alloc(allocator_t *allocator)
{
	obj_deamon_t *obj_deamon;

	obj_deamon = (obj_deamon_t *)allocator_mem_alloc(allocator,sizeof(obj_deamon_t));
	if(obj_deamon == NULL) {
		dbg_str(DBG_DETAIL,"allocator_mem_alloc");
		return obj_deamon;
	}
	memset(obj_deamon,0, sizeof(obj_deamon_t));

	obj_deamon->allocator = allocator;

	return obj_deamon;
}

int obj_deamon_set(obj_deamon_t *obj_deamon, char *attrib, char *value)
{
	if(!strcmp(attrib, "map_type") == 0) {
		obj_deamon->map_type = atoi(value);
	} else {
		dbg_str(DBG_DETAIL,"obj_deamon set, not support %s setting",attrib);
	}

	return 0;
}

int obj_deamon_init(obj_deamon_t *obj_deamon)
{
	if(obj_deamon->map_type == 0) {
		obj_deamon->map_type = MAP_TYPE_HASH_MAP;
	}
	if(obj_deamon->map_value_size == 0) {
		obj_deamon->map_value_size = sizeof(void *);
	}
	if(obj_deamon->map_key_len == 0) {
		obj_deamon->map_key_len = 20;
	}

    obj_deamon->map = (map_t *)map_alloc(obj_deamon->allocator,obj_deamon->map_type);
	if(obj_deamon->map == NULL) {
		dbg_str(DBG_ERROR,"map_alloc");
		return -1;
	}

    map_init(obj_deamon->map, obj_deamon->map_key_len, obj_deamon->map_value_size);

	return 0;
}

int obj_deamon_register_class(obj_deamon_t *obj_deamon,
							  char *class_name,
							  void *class_info_addr)
{
	uint8_t addr_buf[6];

	addr_to_buffer(class_info_addr,addr_buf);
	return map_insert(obj_deamon->map,class_name, addr_buf);
}

void * obj_deamon_search_class(obj_deamon_t *obj_deamon, char *class_name)
{
	map_iterator_t it;
	uint8_t *addr;
	int ret;

	ret = map_search(obj_deamon->map, class_name, &it);
	if(ret < 0) {
		dbg_str(DBG_WARNNING,"obj_deamon_search_method, not found %s",class_name);
		return NULL;
	}

	addr = (uint8_t *)map_get_pointer(&it);

	return buffer_to_addr(addr);
}

obj_deamon_t *obj_deamon_get_global_obj_deamon()
{
	return global_obj_deamon;
}

__attribute__((constructor(CONSTRUCTOR_PRIORITY_OBJ_DEAMON))) void
obj_deamon()
{
	obj_deamon_t *obj_deamon;
	allocator_t *allocator = allocator_get_default_alloc();

	dbg_str(DBG_DETAIL,"obj_deamon");

	obj_deamon = obj_deamon_alloc(allocator);
	obj_deamon_init(obj_deamon);

	global_obj_deamon = obj_deamon;
}


