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

int obj_set(obj_t *obj, char *attrib, void *value)
{
	if(strcmp(attrib, "construct") == 0) {
		obj->construct = value;
		dbg_str(DBG_DETAIL,"**********obj set %s addr %p,obj->construct addr:%p",
				attrib,value,obj->construct);
	} else if(strcmp(attrib, "deconstruct") == 0) {
		obj->deconstruct = value;
		dbg_str(DBG_DETAIL,"*********obj set %s addr %p,obj->deconstruct addr:%p",
				attrib,value,obj->deconstruct);
	} else {
		dbg_str(DBG_DETAIL,"obj set, not support %s setting",attrib);
	}

	return 0;
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

//--------------------------obj method------------------------->>
int obj_construct(struct obj_s *obj,char *init_str)
{
	dbg_str(DBG_DETAIL,"obj construct, obj addr:%p",obj);

	return 0;
}

int obj_deconstrcut(struct obj_s *obj)
{
	dbg_str(DBG_DETAIL,"obj deconstruct,obj addr:%p",obj);

	return 0;
}
//--------------------------------------------------------------<<

static class_info_entry_t obj_class[] = {
	[0] = {ENTRY_TYPE_NORMAL_POINTER,"allocator_t","allocator",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","construct",obj_construct,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",obj_deconstrcut,sizeof(void *)},
	[3] = {ENTRY_TYPE_END},

};

void test_obj()
{
	obj_t *obj;
	allocator_t *allocator = allocator_get_default_alloc();
	obj_deamon_t *deamon = obj_deamon_get_global_obj_deamon();

	dbg_str(DBG_DETAIL,"test_obj");
	dbg_str(DBG_DETAIL,"obj_construct addr:%p",obj_construct);
	dbg_str(DBG_DETAIL,"obj_class addr:%p",obj_class);

	obj_deamon_register_class(deamon,(char *)"obj_class", obj_class);

	obj = obj_alloc(allocator);
	obj_init(obj);
}


