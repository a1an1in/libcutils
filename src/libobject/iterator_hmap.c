/**
 * @file iter.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/iterator_hmap.h>

static int __construct(Iterator *iter,char *init_str)
{
    Hmap_Iterator *hiter;
	dbg_str(DBG_SUC,"Hmap_Iterator construct, iter addr:%p",iter);

	return 0;
}

static int __deconstrcut(Iterator *iter)
{
    Hmap_Iterator *hiter;
	dbg_str(DBG_SUC,"Hmap_Iterator deconstruct,iter addr:%p",iter);

	return 0;
}

static int __set(Iterator *iter, char *attrib, void *value)
{
    Hmap_Iterator *hiter = (Hmap_Iterator *)iter;
	if(strcmp(attrib, "set") == 0) {
		hiter->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		hiter->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		hiter->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		hiter->deconstruct = value;
	} else if(strcmp(attrib, "next") == 0) {
		hiter->next = value;
	} else if(strcmp(attrib, "prev") == 0) {
		hiter->prev = value;
	} else if(strcmp(attrib, "equal") == 0) {
		hiter->equal = value;
	} else if(strcmp(attrib, "get_dpointer") == 0) {
		hiter->get_dpointer = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(hiter->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"hiter set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Iterator *iter, char *attrib)
{
    Hmap_Iterator *hiter = (Hmap_Iterator *)iter;

    if(strcmp(attrib, "name") == 0) {
        return hiter->name;
    } else {
        dbg_str(DBG_WARNNING,"iter get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __next(Iterator *it, Iterator *next)
{
	dbg_str(DBG_SUC,"Hmap_Iterator next");

    hash_map_pos_next(&((Hmap_Iterator *)it)->hash_map_pos,
                      &((Hmap_Iterator *)next)->hash_map_pos);

}

static int __prev(Iterator *it, Iterator *prev)
{
    Hmap_Iterator *hiter = (Hmap_Iterator *)it;
	dbg_str(DBG_SUC,"Hmap_Iterator prev,this func is not implemented");
}

static int __equal(Iterator *it1,Iterator *it2)
{
	dbg_str(DBG_SUC,"Hmap_Iterator equal");

    return hash_map_pos_equal(&((Hmap_Iterator *)it1)->hash_map_pos,
                              &((Hmap_Iterator *)it2)->hash_map_pos);
}

static void *__get_dpointer(Iterator *it)
{
	dbg_str(DBG_SUC,"Hmap_Iterator get_dpointer");
    return hash_map_pos_get_pointer(&((Hmap_Iterator *)it)->hash_map_pos);
}

static class_info_entry_t hmap_iter_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Iterator","iter",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_FUNC_POINTER,"","next",__next,sizeof(void *)},
	[6] = {ENTRY_TYPE_FUNC_POINTER,"","prev",__prev,sizeof(void *)},
	[7] = {ENTRY_TYPE_FUNC_POINTER,"","equal",__equal,sizeof(void *)},
	[8] = {ENTRY_TYPE_FUNC_POINTER,"","get_dpointer",__get_dpointer,sizeof(void *)},
	[9] = {ENTRY_TYPE_END},
};
REGISTER_CLASS("Hmap_Iterator",hmap_iter_class_info);

void test_obj_hiter()
{
    Iterator *iter, *next,*prev;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str = NULL;
    cjson_t *root, *e, *s;
    char buf[2048];

    iter = OBJECT_NEW(allocator, Hmap_Iterator,set_str);
    next = OBJECT_NEW(allocator, Hmap_Iterator,set_str);

    iter->next(iter,next);
}


