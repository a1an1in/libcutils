#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/object.h>

void * object_get_set_func_pointer(void *class_info_addr)
{
	class_info_entry_t *entry = (class_info_entry_t *)class_info_addr;
	int i;

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		if(		strcmp((char *)entry[i].value_name,"set") == 0 && 
				entry[i].type == ENTRY_TYPE_FUNC_POINTER) {
			return entry[i].value;
		}
	}	

	return 0;
}

void * object_get_construct_func_pointer(void *class_info_addr)
{
	class_info_entry_t *entry = (class_info_entry_t *)class_info_addr;
	int i;

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		if(		strcmp((char *)entry[i].value_name,"construct") == 0 && 
				entry[i].type == ENTRY_TYPE_FUNC_POINTER) {
			return entry[i].value;
		}
	}	

	return 0;
}

class_info_entry_t * object_get_subclass_info(void *class_info_addr)
{
	class_info_entry_t *entry = (class_info_entry_t *)class_info_addr;
	int i;

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		if( entry[i].type == ENTRY_TYPE_OBJ) {
			return &entry[i];
		}
	}	

	return NULL;
}

int object_init_func_pointer(void *obj,void *class_info_addr)
{
	class_info_entry_t *entry = (class_info_entry_t *)class_info_addr;
	int i;

	int (*set)(void *obj, char *attrib, void *value);

 	set = object_get_set_func_pointer(class_info_addr);
	if(set == NULL) {
		dbg_str(DBG_WARNNING,"obj_init_func_pointer,set func is NULL");
		return -1;
	}

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		dbg_str(DBG_DETAIL,"value_name %s, value %p",
				entry[i].value_name,
				entry[i].value);
		set(obj, (char *)entry[i].value_name, entry[i].value);
	}	

	return 0;
}

void *
object_find_reimplement_func_pointer(char *method_name,
									 char *start_type_name,
									 char *end_type_name)
{
	class_info_entry_t *entry;
	object_deamon_t *deamon;
	char *subclass_name = NULL;
	int i;

	if(strcmp(start_type_name,end_type_name) == 0) return NULL;
	if(start_type_name == NULL) {
		dbg_str(DBG_WARNNING,"object_find_reimplement_func_pointer, start addr is NULL");
		return NULL;
	}

	deamon = object_deamon_get_global_object_deamon();
	entry  = (class_info_entry_t *)object_deamon_search_class(deamon,
															  (char *)start_type_name);
	if(entry[0].type == ENTRY_TYPE_OBJ) {
		subclass_name = entry[0].type_name;
	}
	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		if( 	entry[i].type == ENTRY_TYPE_FUNC_POINTER &&
				strcmp(entry[i].value_name, method_name) == 0)
		{
			return entry[i].value;
		}

	}	

	return object_find_reimplement_func_pointer(method_name,
										        subclass_name,
										        end_type_name);
}

int object_cover_vitual_func_pointer(void *obj,
									 char *cur_type_name,
									 char *type_name)
{
	class_info_entry_t *entry;
	object_deamon_t *deamon;
	int i;
	int (*set)(void *obj, char *attrib, void *value);
	void *reimplement_func;

	if(strcmp(cur_type_name,type_name) == 0) return 0;

	deamon = object_deamon_get_global_object_deamon();
	entry  = (class_info_entry_t *)
             object_deamon_search_class(deamon, (char *)cur_type_name);

 	set    = object_get_set_func_pointer(entry);
	if(set == NULL) {
		dbg_str(DBG_WARNNING,"obj_init_func_pointer,set func is NULL");
		return -1;
	}

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		if(entry[i].type == ENTRY_TYPE_VIRTUAL_FUNC_POINTER){
			reimplement_func = object_find_reimplement_func_pointer(entry[i].value_name,
																	type_name,
																	cur_type_name);
			set(obj, (char *)entry[i].value_name, reimplement_func);
		}
	}	

	return 0;
}

int __object_init(void *obj, char *cur_type_name, char *type_name) 
{
	object_deamon_t *deamon;
	void *class_info_addr;
 	class_info_entry_t * subclass_info_addr;
	int (*construct)(void *obj,char *init_str);

	dbg_str(DBG_DETAIL,"current obj type name =%s",cur_type_name);

	deamon             = object_deamon_get_global_object_deamon();
	class_info_addr    = object_deamon_search_class(deamon,(char *)cur_type_name);
	construct          = object_get_construct_func_pointer(class_info_addr);
 	subclass_info_addr = object_get_subclass_info(class_info_addr);

	dbg_str(DBG_DETAIL,"obj_class addr:%p",class_info_addr);
 	if(subclass_info_addr != NULL) {
		dbg_str(DBG_DETAIL,"init subclass");
		__object_init(obj, subclass_info_addr->type_name, type_name);
 	} else {
		dbg_str(DBG_DETAIL,"obj has not subclass");
	}

	object_init_func_pointer(obj,class_info_addr);
	object_cover_vitual_func_pointer(obj, cur_type_name, type_name);

	dbg_str(DBG_DETAIL,"obj addr:%p",obj);
	construct(obj,NULL);

	return 0;
}

int object_init(void *obj, char *type_name) 
{
	__object_init(obj, type_name, type_name);
}

int obj_construct(Obj *obj,char *init_str)
{
	dbg_str(DBG_SUC,"obj construct, obj addr:%p",obj);

	return 0;
}

int obj_deconstrcut(Obj *obj)
{
	dbg_str(DBG_SUC,"obj deconstruct,obj addr:%p",obj);

	return 0;
}

int obj_set(Obj *obj, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		obj->set = value;
	} else if(strcmp(attrib, "construct") == 0) {
		obj->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		obj->deconstruct = value;
	} else {
		dbg_str(DBG_DETAIL,"obj set, not support %s setting",attrib);
	}

	return 0;
}

static class_info_entry_t obj_class_info[] = {
	[0] = {ENTRY_TYPE_NORMAL_POINTER,"allocator_t","allocator",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",obj_set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","construct",obj_construct,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",obj_deconstrcut,sizeof(void *)},
	[4] = {ENTRY_TYPE_END},

};

__attribute__((constructor(CONSTRUCTOR_PRIORITY_REGISTER_CLASS))) static void
register_class()
{
	object_deamon_t *deamon = object_deamon_get_global_object_deamon();

    printf("CONSTRUCTOR_PRIORITY_REGISTER_CLASS=%d,class name %s\n",
			CONSTRUCTOR_PRIORITY_REGISTER_CLASS, "Obj");

	object_deamon_register_class(deamon,(char *)"Obj", obj_class_info);
}

void test_obj()
{
	Obj *obj;
	allocator_t *allocator = allocator_get_default_alloc();

    obj = OBJECT_NEW(allocator, Obj,"");
}


