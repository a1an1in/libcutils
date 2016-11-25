/**
 * @file component.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/component.h>

static int component_construct(Component *component,char *init_str)
{
	dbg_str(DBG_SUC,"component construct, component addr:%p",component);

	return 0;
}

static int component_deconstrcut(Component *component)
{
	dbg_str(DBG_SUC,"component deconstruct,component addr:%p",component);

	return 0;
}

static int component_move(Component *component)
{
	dbg_str(DBG_SUC,"component move");
}

static int component_set(Component *component, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		component->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		component->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		component->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		component->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
		component->move = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(component->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"component set, not support %s setting",attrib);
	}

	return 0;
}

void *component_get(Component *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"component get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t component_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Container","container",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",component_set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",component_get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",component_construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",component_deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_FUNC_POINTER,"","move",component_move,sizeof(void *)},
	[6] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[7] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Component",component_class_info);

void test_ui_component()
{
    Subject *subject;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    cjson_t *root, *e, *s;
    char buf[2048];

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Component", e = cjson_create_object());{
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
     *subject = OBJECT_ALLOC(allocator,Component);
     *object_set(subject, "Component", set_str);
     *dbg_str(DBG_DETAIL,"x=%d y=%d width=%d height=%d",subject->x,subject->y,subject->width,subject->height);
     */

    subject = OBJECT_NEW(allocator, Component,set_str);

    /*
     *dbg_str(DBG_DETAIL,"x=%d y=%d width=%d height=%d",subject->x,subject->y,subject->width,subject->height);
     *dbg_str(DBG_DETAIL,"component nane=%s",((Component *)subject)->name);
     *subject->move(subject);
     */

    object_dump(subject, "Component", buf, 2048);
    dbg_str(DBG_DETAIL,"Component dump: %s",buf);

    free(set_str);

}


