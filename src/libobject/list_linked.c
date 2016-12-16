/**
 * @file list.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/list_hash.h>

static int __construct(List *list,char *init_str)
{

	return 0;
}

static int __deconstrcut(List *list)
{
	dbg_str(OBJ_DETAIL,"hash list deconstruct,list addr:%p",list);

	return 0;
}

static int __set(List *m, char *attrib, void *value)
{
    Hash_List *list = (Hash_List *)m;

	if(strcmp(attrib, "set") == 0) {
		list->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		list->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		list->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		list->deconstruct = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(list->name,value,strlen(value));
	} else if(strcmp(attrib, "insert") == 0) {
		list->insert = value;
	} else if(strcmp(attrib, "insert_wb") == 0) {
		list->insert_wb = value;
	} else if(strcmp(attrib, "search") == 0) {
		list->search = value;
	} else if(strcmp(attrib, "del") == 0) {
		list->del = value;
	} else if(strcmp(attrib, "for_each") == 0) {
        /*
         *dbg_str(DBG_DETAIL,"hash list set for each addr:%p",value);
         */
		list->for_each = value;
	} else if(strcmp(attrib, "begin") == 0) {
		list->begin = value;
	} else if(strcmp(attrib, "end") == 0) {
		list->end = value;
	} else if(strcmp(attrib, "destroy") == 0) {
		list->destroy = value;
	} else if(strcmp(attrib, "key_size") == 0) {
        list->key_size = *((uint16_t *)value);
	} else if(strcmp(attrib, "value_size") == 0) {
        list->value_size = *((uint16_t *)value);
	} else if(strcmp(attrib, "bucket_size") == 0) {
        list->bucket_size = *((uint16_t *)value);
	} else {
		dbg_str(OBJ_WARNNING,"list set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(List *obj, char *attrib)
{

    Hash_List *list = (Hash_List *)obj;

    if(strcmp(attrib, "name") == 0) {
        return list->name;
	} else if(strcmp(attrib, "key_size") == 0) {
        return &list->key_size;
	} else if(strcmp(attrib, "value_size") == 0) {
        return &list->value_size;
	} else if(strcmp(attrib, "bucket_size") == 0) {
        return &list->bucket_size;
    } else {
        dbg_str(OBJ_WARNNING,"hash list get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __insert(List *list,void *key,void *value)
{
	dbg_str(OBJ_DETAIL,"Hash List insert");

    return hash_list_insert(((Hash_List *)list)->hlist,key,value);
}

static int __insert_wb(List *list,void *key,void *value,Iterator *iter)
{
	dbg_str(OBJ_DETAIL,"Hash List insert wb");
    return hash_list_insert_wb(((Hash_List *)list)->hlist,key,value,
                              &((Hlist_Iterator *)iter)->hash_list_pos);
}

static int __search(List *list,void *key,Iterator *iter)
{
	dbg_str(OBJ_IMPORTANT,"Hash List search");
    return hash_list_search(((Hash_List *)list)->hlist, key,
                           &((Hlist_Iterator *)iter)->hash_list_pos);
}

static int __del(List *list,Iterator *iter)
{
	dbg_str(OBJ_DETAIL,"Hash List del");
    return hash_list_delete(((Hash_List *)list)->hlist,
                            &((Hlist_Iterator *)iter)->hash_list_pos);
}

static Iterator *__begin(List *list)
{
    Iterator *iter;
    allocator_t *allocator = list->obj.allocator;

	dbg_str(OBJ_DETAIL,"Hash List begin");

    iter = OBJECT_NEW(allocator, Hlist_Iterator,NULL);

    hash_list_begin(((Hash_List *)list)->hlist,
                    &((Hlist_Iterator *)iter)->hash_list_pos);

    return iter;
}

static Iterator *__end(List *list)
{
    Iterator *iter;
    allocator_t *allocator = list->obj.allocator;

	dbg_str(OBJ_DETAIL,"Hash List end");
    iter = OBJECT_NEW(allocator, Hlist_Iterator,NULL);

    hash_list_end(((Hash_List *)list)->hlist,
                 &((Hlist_Iterator *)iter)->hash_list_pos);

    return iter;
}

static class_info_entry_t hash_list_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"List","list",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","insert",__insert,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_FUNC_POINTER,"","insert_wb",__insert_wb,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_FUNC_POINTER,"","search",__search,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_FUNC_POINTER,"","del",__del,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_FUNC_POINTER,"","begin",__begin,sizeof(void *)},
	[10] = {ENTRY_TYPE_FUNC_POINTER,"","end",__end,sizeof(void *)},
	[11] = {ENTRY_TYPE_UINT16_T,"","key_size",NULL,sizeof(short)},
	[12] = {ENTRY_TYPE_UINT16_T,"","value_size",NULL,sizeof(short)},
	[13] = {ENTRY_TYPE_UINT16_T,"","bucket_size",NULL,sizeof(short)},
	[14] = {ENTRY_TYPE_FUNC_POINTER,"","for_each",NULL,sizeof(void *)},
	[15] = {ENTRY_TYPE_END},
};
REGISTER_CLASS("Hash_List",hash_list_class_info);

static void hash_list_print(Iterator *iter)
{
    Hlist_Iterator *i = (Hlist_Iterator *)iter;
    dbg_str(DBG_DETAIL,"key:%s value:%s",i->get_kpointer(iter), i->get_vpointer(iter));
}

void test_obj_hash_list()
{
    Iterator *iter, *next,*prev;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    cjson_t *root, *e, *s;
    char buf[2048];

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Hash_List", e = cjson_create_object());{
            cjson_add_number_to_object(e, "key_size", 10);
            cjson_add_number_to_object(e, "value_size", 25);
            cjson_add_number_to_object(e, "bucket_size", 15);
        }
    }
    set_str = cjson_print(root);

#if 1
    List *list;
    list  = OBJECT_NEW(allocator, Hash_List,set_str);
    iter = OBJECT_NEW(allocator, Hlist_Iterator,set_str);

    object_dump(list, "Hash_List", buf, 2048);
    dbg_str(DBG_DETAIL,"List dump: %s",buf);

    list->insert(list,"abc","hello world");
    list->insert(list,"test","sdfsafsdaf");
    list->for_each(list,hash_list_print);

    list->search(list,"abc",iter);
    dbg_str(DBG_DETAIL,"search data:%s",iter->get_vpointer(iter));
    list->del(list,iter);

    list->for_each(list,hash_list_print);

#else
    Hash_List *list;
    list  = OBJECT_NEW(allocator, Hash_List,set_str);
    iter = OBJECT_NEW(allocator, Hlist_Iterator,set_str);

    object_dump(list, "Hash_List", buf, 2048);
    dbg_str(OBJ_DETAIL,"List dump: %s",buf);

    list->insert((List *)list,"abc","hello world");
    list->insert((List *)list,"test","sdfsafsdaf");
    list->for_each((List *)list,hash_list_print);

#endif
    object_destroy(list);
    object_destroy(iter);
    free(set_str);
}


