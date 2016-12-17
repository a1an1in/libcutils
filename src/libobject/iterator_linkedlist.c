/**
 * @file iter.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/iterator_linkedlist.h>

static int __set(Iterator *iter, char *attrib, void *value)
{
    LList_Iterator *li = (LList_Iterator *)iter;

	if(strcmp(attrib, "set") == 0) {
		li->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		li->get = value;
	} else if(strcmp(attrib, "next") == 0) {
		li->next = value;
	} else if(strcmp(attrib, "prev") == 0) {
		li->prev = value;
	} else if(strcmp(attrib, "equal") == 0) {
		li->equal = value;
	} else if(strcmp(attrib, "get_vpointer") == 0) {
		li->get_vpointer = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(li->name,value,strlen(value));
	} else {
		dbg_str(OBJ_DETAIL,"li set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Iterator *iter, char *attrib)
{
    LList_Iterator *li = (LList_Iterator *)iter;

    if(strcmp(attrib, "name") == 0) {
        return li->name;
    } else {
        dbg_str(OBJ_WARNNING,"iter get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static Iterator *__next(Iterator *it)
{
    Iterator *next = it;

	dbg_str(OBJ_DETAIL,"LList_Iterator next");

	llist_pos_next(&((LList_Iterator *)it)->list_pos,
				   &((LList_Iterator *)next)->list_pos);

    return next;

}

static Iterator *__prev(Iterator *it)
{
    Iterator *prev = it;

	dbg_str(OBJ_DETAIL,"LList_Iterator prev");

	llist_pos_prev(&((LList_Iterator *)it)->list_pos,
				   &((LList_Iterator *)prev)->list_pos);
}

static int __equal(Iterator *it1,Iterator *it2)
{
	dbg_str(OBJ_DETAIL,"LList_Iterator equal");

    return llist_pos_equal(&((LList_Iterator *)it1)->list_pos,
                           &((LList_Iterator *)it2)->list_pos);
}

static void *__get_vpointer(Iterator *it)
{
	dbg_str(OBJ_DETAIL,"LList_Iterator get_vpointer");
    return llist_pos_get_pointer(&((LList_Iterator *)it)->list_pos);
}

static class_info_entry_t hmap_iter_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Iterator","iter",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","next",__next,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","prev",__prev,sizeof(void *)},
	[5] = {ENTRY_TYPE_FUNC_POINTER,"","equal",__equal,sizeof(void *)},
	[6] = {ENTRY_TYPE_FUNC_POINTER,"","get_vpointer",__get_vpointer,sizeof(void *)},
	[7] = {ENTRY_TYPE_END},
};
REGISTER_CLASS("LList_Iterator",hmap_iter_class_info);

void test_obj_linked_list_iter()
{
    Iterator *iter, *next,*prev;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str = NULL;
    cjson_t *root, *e, *s;
    char buf[2048];

    iter = OBJECT_NEW(allocator, LList_Iterator,set_str);
    next = OBJECT_NEW(allocator, LList_Iterator,set_str);

    iter->next(iter);
}


