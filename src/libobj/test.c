#include <stdio.h>
#include <libdbg/debug.h>
#include <libobj/obj.h>

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


