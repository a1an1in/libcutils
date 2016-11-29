/**
 * @file map.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/map.h>

static int __construct(Map *map,char *init_str)
{
	dbg_str(DBG_SUC,"map construct, map addr:%p",map);

	return 0;
}

static int __deconstrcut(Map *map)
{
	dbg_str(DBG_SUC,"map deconstruct,map addr:%p",map);

	return 0;
}

static int __set(Map *map, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		map->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		map->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		map->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		map->deconstruct = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(map->name,value,strlen(value));
	} else if(strcmp(attrib, "insert") == 0) {
		map->insert = value;
	} else if(strcmp(attrib, "insert_wb") == 0) {
		map->insert_wb = value;
	} else if(strcmp(attrib, "search") == 0) {
		map->search = value;
	} else if(strcmp(attrib, "del") == 0) {
		map->del = value;
	} else if(strcmp(attrib, "for_each") == 0) {
		map->for_each = value;
	} else if(strcmp(attrib, "begin") == 0) {
		map->begin = value;
	} else if(strcmp(attrib, "end") == 0) {
		map->end = value;
	} else if(strcmp(attrib, "destroy") == 0) {
		map->destroy = value;
	} else {
		dbg_str(DBG_DETAIL,"map set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Map *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"map get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __insert(Map *map,void *key,void *value)
{
	dbg_str(DBG_SUC,"Map insert");
}

static int __insert_wb(Map *map,void *key,void *value,Iterator *iter)
{
	dbg_str(DBG_SUC,"Map insert wb");
}

static int __search(Map *map,void *key,Iterator *iter)
{
	dbg_str(DBG_SUC,"Map search");
}

static int __del(Map *map,Iterator *iter)
{
	dbg_str(DBG_SUC,"Map del");
}

static void __for_each(Map *map,void (*func)(Map *map, char *key, void *value))
{
	dbg_str(DBG_SUC,"Mapfor_each");

}

static int __begin(Map *map,Iterator *begin)
{
	dbg_str(DBG_SUC,"Map begin");
}

static int __end(Map *map,Iterator *end)
{
	dbg_str(DBG_SUC,"Map end");
}

static int __destroy(Map *map)
{
	dbg_str(DBG_SUC,"Map destroy");
}

static class_info_entry_t map_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","insert",__insert,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","insert_wb",__insert_wb,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","search",__search,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","del",__del,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","for_each",__for_each,sizeof(void *)},
	[10] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","begin",__begin,sizeof(void *)},
	[11] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","end",__end,sizeof(void *)},
	[12] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","destroy",__destroy,sizeof(void *)},
	[13] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Map",map_class_info);

void test_obj_map()
{
    Map *map;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    cjson_t *root, *e, *s;
    char buf[2048];

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Map", e = cjson_create_object());{
            cjson_add_string_to_object(e, "name", "alan");
        }
    }

    set_str = cjson_print(root);

    map = OBJECT_NEW(allocator, Map,set_str);


    object_dump(map, "Map", buf, 2048);
    dbg_str(DBG_DETAIL,"Map dump: %s",buf);

    free(set_str);

}


