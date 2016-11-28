/**
 * @file container.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/container.h>

static int __construct(Container *container,char *init_str)
{
	dbg_str(DBG_SUC,"container construct, container addr:%p",container);

	return 0;
}

static int __deconstrcut(Container *container)
{
	dbg_str(DBG_SUC,"container deconstruct,container addr:%p",container);

	return 0;
}

static int __move(Container *container)
{
	dbg_str(DBG_SUC,"container move");
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
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(container->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"container set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Container *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"container get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t container_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Subject","subject",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_FUNC_POINTER,"","move",__move,sizeof(void *)},
	[6] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[7] = {ENTRY_TYPE_END},

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


