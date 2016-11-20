#include <stdio.h>
#include <libdbg/debug.h>
#include <libobj/obj.h>

obj_t * obj_alloc(allocator_t *allocator)
{
	obj_t *obj;

	obj = (obj_t *)allocator_mem_alloc(allocator,sizeof(obj_t));
	if(obj == NULL) {
		dbg_str(DBG_DETAIL,"allocator_mem_alloc");
		return obj;
	}
	memset(obj,0, sizeof(obj_t));

	obj->allocator = allocator;

	return obj;
}

int obj_init_func_pointer(obj_t *obj,void *class_info_addr)
{
	class_info_entry_t *entry = (class_info_entry_t *)class_info_addr;
	int i;

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		dbg_str(DBG_DETAIL,"value_name %s, value %p",
				entry[i].value_name,
				entry[i].value);
		obj_set(obj, (char *)entry[i].value_name, entry[i].value);
	}	

	return 0;
}

int obj_init(obj_t *obj)
{
	obj_deamon_t *deamon;
	void *class_info_addr;

	deamon = obj_deamon_get_global_obj_deamon();

	class_info_addr = obj_deamon_search_class(deamon,(char *)"obj_class");
	dbg_str(DBG_DETAIL,"obj_class addr:%p",class_info_addr);

	obj_init_func_pointer(obj,class_info_addr);

	dbg_str(DBG_DETAIL,"obj addr:%p",obj);
	dbg_str(DBG_DETAIL,"construct addr:%p",obj->construct);
	dbg_str(DBG_DETAIL,"deconstruct addr:%p",obj->deconstruct);
	obj->construct(obj,NULL);

	return 0;
}

