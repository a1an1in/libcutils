/**
 * @file label.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/label.h>
#include <libui/sdl_window.h>

static int __construct(Label *label,char *init_str)
{
	dbg_str(DBG_SUC,"label construct, label addr:%p",label);

	return 0;
}

static int __deconstrcut(Label *label)
{
	dbg_str(DBG_SUC,"label deconstruct,label addr:%p",label);

	return 0;
}

static int __move(Label *label)
{
	dbg_str(DBG_SUC,"label move");
}

static int __set(Label *label, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		label->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		label->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		label->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		label->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
		label->move = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(label->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"label set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Label *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"label get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t label_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_FUNC_POINTER,"","move",__move,sizeof(void *)},
	[6] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[7] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Label",label_class_info);

char *gen_label_setting_str()
{
    cjson_t *root,*b, *c, *e, *s;
    char *set_str;

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Label", b = cjson_create_object());{
            cjson_add_item_to_object(root, "Component", c = cjson_create_object());{
                cjson_add_item_to_object(root, "Container", e = cjson_create_object());{
                    cjson_add_item_to_object(e, "Subject", s = cjson_create_object());{
                        cjson_add_number_to_object(s, "x", 1);
                        cjson_add_number_to_object(s, "y", 25);
                        cjson_add_number_to_object(s, "width", 80);
                        cjson_add_number_to_object(s, "height", 30);
                    }
                }
            }
            cjson_add_string_to_object(c, "name", "label");
        }
    }
    set_str = cjson_print(root);

    return set_str;
}

void test_ui_label()
{
    Window *window;
    Container *container;
	Graph *g;
    Subject *subject;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    char buf[2048];

    set_str   = gen_window_setting_str();
    window    = OBJECT_NEW(allocator, Sdl_Window,set_str);
	g         = window->graph;
    container = (Container *)window;
    object_dump(window, "Sdl_Window", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

    set_str = gen_label_setting_str();
    subject = OBJECT_NEW(allocator, Label,set_str);
    object_dump(subject, "Label", buf, 2048);
    dbg_str(DBG_DETAIL,"Label dump: %s",buf);

    container->add_component(container,subject);
    container->search_component(container,"label");

    free(set_str);

}


