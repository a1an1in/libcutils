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

static int __map_construct(Map *map,char *init_str)
{
	dbg_str(DBG_SUC,"map construct, map addr:%p",map);

	return 0;
}

static int __map_deconstrcut(Map *map)
{
	dbg_str(DBG_SUC,"map deconstruct,map addr:%p",map);

	return 0;
}

static int __map_set(Map *map, char *attrib, void *value)
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
	} else {
		dbg_str(DBG_DETAIL,"map set, not support %s setting",attrib);
	}

	return 0;
}

void *__map_get(Map *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"map get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t map_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__map_set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__map_get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__map_construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__map_deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_END},

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

    /*
     *map = OBJECT_ALLOC(allocator,Map);
     *object_set(map, "Map", set_str);
     *dbg_str(DBG_DETAIL,"x=%d y=%d width=%d height=%d",map->x,map->y,map->width,map->height);
     */

    map = OBJECT_NEW(allocator, Map,set_str);

    /*
     *dbg_str(DBG_DETAIL,"x=%d y=%d width=%d height=%d",map->x,map->y,map->width,map->height);
     *dbg_str(DBG_DETAIL,"map nane=%s",((Map *)map)->name);
     *map->move(map);
     */

    object_dump(map, "Map", buf, 2048);
    dbg_str(DBG_DETAIL,"Map dump: %s",buf);

    free(set_str);

}


