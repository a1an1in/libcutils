/**
 * @file container.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/container.h>
#include <libui/component.h>
#include <libobject/map_hash.h>
#include <miscellany/buffer.h>

static int __construct(Container *container,char *init_str)
{
    allocator_t *allocator = ((Obj *)container)->allocator;
    Map *map;

	dbg_str(DBG_DETAIL,"container construct, container addr:%p",container);

    if(container->map_type == 1) {
        container->map  = (Map *)OBJECT_NEW(allocator, Hash_Map,container->map_construct_str);
    } else {
        dbg_str(DBG_WARNNING,"not supported map type, type =%d", container->map_type);
        return -1;
    }

	return 0;
}

static int __deconstrcut(Container *container)
{
	dbg_str(DBG_DETAIL,"container deconstruct,container addr:%p",container);
    object_destroy(container->map);

	return 0;
}

static int __set(Container *container, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		container->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		container->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		container->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		container->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
		container->move = value;
	} else if(strcmp(attrib, "add_component") == 0) {
		container->add_component = value;
	} else if(strcmp(attrib, "search_component") == 0) {
		container->search_component = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(container->name,value,strlen(value));
	} else if(strcmp(attrib, "map_type") == 0) {
		container->map_type = *(uint8_t *)value;
	} else {
		dbg_str(DBG_DETAIL,"container set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Container *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else if(strcmp(attrib, "map_type") == 0) {
        return &obj->map_type;
    } else {
        dbg_str(DBG_WARNNING,"container get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __move(Container *container)
{
	dbg_str(DBG_DETAIL,"container move");
}

static int __add_component(Container *obj, Component *component)
{

    if(obj->map_type == 0) {
        dbg_str(DBG_WARNNING,"%s is support container add op",((Obj *)obj)->name);
        return -1;
    }
    dbg_str(DBG_IMPORTANT, "add component name %s, component addr %p", component->name,component);
    if(strcmp(component->name,"") == 0) {
        dbg_str(DBG_WARNNING,"component name is NULL, this is vip, add component failed, please check");
        return -1;
    }

    char buffer[8] = {0};
    addr_to_buffer(component,buffer);

    obj->map->insert(obj->map, component->name, buffer);

    return 0;
}

static Component *__search_component(Container *obj, char *key)
{
    allocator_t *allocator = ((Obj *)obj)->allocator;
    Iterator *iter;
    Map *map = obj->map;
    int ret;
    void *buf_addr, *addr;

    if(obj->map_type == 0) {
        dbg_str(DBG_WARNNING,"%s is support container search op",((Obj *)obj)->name);
        return NULL;
    }
    iter = OBJECT_NEW(allocator, Hmap_Iterator,NULL);
    ret = map->search(map,key,iter);
    if(ret == 1) {
        addr = buffer_to_addr(iter->get_vpointer(iter));
        dbg_str(DBG_IMPORTANT,"search component %s addr %p",iter->get_kpointer(iter), addr);
    } else {
        dbg_str(DBG_DETAIL,"not find component %s",key);
    }

    return addr;
}

static class_info_entry_t container_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Subject","subject",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","move",__move,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_FUNC_POINTER,"","add_component",__add_component,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_FUNC_POINTER,"","search_component",__search_component,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[9 ] = {ENTRY_TYPE_UINT8_T,"uint8_t","map_type",NULL,sizeof(int)},
	[10] = {ENTRY_TYPE_END},
};
REGISTER_CLASS("Container",container_class_info);

void test_ui_container()
{
    Subject *subject;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    cjson_t *root, *e, *s;
    char buf[2048];

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Container", e = cjson_create_object());{
            cjson_add_item_to_object(e, "Subject", s = cjson_create_object());{
                cjson_add_number_to_object(s, "x", 1);
                cjson_add_number_to_object(s, "y", 25);
                cjson_add_number_to_object(s, "width", 5);
                cjson_add_number_to_object(s, "height", 125);
            }
            cjson_add_string_to_object(e, "name", "alan");
        }
    }

    set_str = cjson_print(root);

    /*
     *subject = OBJECT_ALLOC(allocator,Container);
     *object_set(subject, "Container", set_str);
     *dbg_str(DBG_DETAIL,"x=%d y=%d width=%d height=%d",subject->x,subject->y,subject->width,subject->height);
     */

    subject = OBJECT_NEW(allocator, Container,set_str);

    /*
     *dbg_str(DBG_DETAIL,"x=%d y=%d width=%d height=%d",subject->x,subject->y,subject->width,subject->height);
     *dbg_str(DBG_DETAIL,"container nane=%s",((Container *)subject)->name);
     *subject->move(subject);
     */

    object_dump(subject, "Container", buf, 2048);
    dbg_str(DBG_DETAIL,"Container dump: %s",buf);

    free(set_str);

}


