/**
 * @file iter.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/iterator.h>

static int __construct(Iterator *iter,char *init_str)
{
	dbg_str(OBJ_DETAIL,"iter construct, iter addr:%p",iter);

	return 0;
}

static int __deconstrcut(Iterator *iter)
{
	dbg_str(OBJ_DETAIL,"iter deconstruct,iter addr:%p",iter);

	return 0;
}

static int __set(Iterator *iter, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		iter->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		iter->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		iter->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		iter->deconstruct = value;
	} else if(strcmp(attrib, "next") == 0) {
		iter->next = value;
	} else if(strcmp(attrib, "prev") == 0) {
		iter->prev = value;
	} else if(strcmp(attrib, "equal") == 0) {
		iter->equal = value;
	} else if(strcmp(attrib, "get_dpointer") == 0) {
		iter->get_dpointer = value;
	} else if(strcmp(attrib, "destroy") == 0) {
		iter->destroy = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(iter->name,value,strlen(value));
	} else {
		dbg_str(OBJ_DETAIL,"iter set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Iterator *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(OBJ_WARNNING,"iter get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static Iterator *__next(Iterator *it)
{
	dbg_str(OBJ_DETAIL,"Iterator next");
}

static Iterator *__prev(Iterator *it)
{
	dbg_str(OBJ_DETAIL,"Iterator prev");
}

static int __equal(Iterator *it1,Iterator *it2)
{
	dbg_str(OBJ_DETAIL,"Iterator equal");
}

static void *__get_dpointer(Iterator *it)
{
	dbg_str(OBJ_DETAIL,"Iterator get_dpointer");
}

static int __destroy(Iterator *it)
{
	dbg_str(OBJ_DETAIL,"Iterator destroy");
    allocator_mem_free(it->obj.allocator,it);
}

static class_info_entry_t iter_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","next",__next,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","prev",__prev,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","equal",__equal,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","get_dpointer",__get_dpointer,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","destroy",__destroy,sizeof(void *)},
	[10] = {ENTRY_TYPE_END},
};
REGISTER_CLASS("Iterator",iter_class_info);

void test_obj_iter()
{
    Iterator *iter, *next,*prev;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str = NULL;
    cjson_t *root, *e, *s;
    char buf[2048];

    iter = OBJECT_NEW(allocator, Iterator,set_str);
    /*
     *next = OBJECT_NEW(allocator, Iterator,set_str);
     *prev = OBJECT_NEW(allocator, Iterator,set_str);
     */

    iter->next(iter);
    iter->prev(iter);
}


