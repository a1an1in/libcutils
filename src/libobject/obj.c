/**
 * @file object.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/object.h>

static int __construct(Obj *obj,char *init_str)
{
	dbg_str(OBJ_DETAIL,"obj construct, obj addr:%p",obj);

	return 0;
}

static int __deconstrcut(Obj *obj)
{
	dbg_str(OBJ_DETAIL,"obj deconstruct,obj addr:%p",obj);
    allocator_mem_free(obj->allocator,obj);

	return 0;
}

static int __set(Obj *obj, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		obj->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		obj->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		obj->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		obj->deconstruct = value;
    } else if(strcmp(attrib, "allocator") == 0) {
        obj->allocator = value;
        dbg_str(OBJ_DETAIL,"allocator addr:%p",obj->allocator);
	} else {
		dbg_str(OBJ_WARNNING,"obj set, \"%s\" setting is not supported",attrib);
	}

	return 0;
}

static void *__get(Obj *obj, char *attrib)
{
    if(strcmp(attrib, "allocator") == 0) {
        return obj->allocator;
    } else {
        dbg_str(OBJ_WARNNING,"obj get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t obj_class_info[] = {
	[0] = {ENTRY_TYPE_NORMAL_POINTER,"allocator_t","allocator",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_END},
};
REGISTER_CLASS("Obj",obj_class_info);

void test_obj()
{
	Obj *obj;
    char buf[2048];
	allocator_t *allocator = allocator_get_default_alloc();

    obj = OBJECT_NEW(allocator, Obj,"");

    object_dump(obj, "Obj", buf, 2048);
}


