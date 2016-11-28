/**
 * @file button.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/button.h>

static int button_construct(Button *button,char *init_str)
{
	dbg_str(DBG_SUC,"button construct, button addr:%p",button);

	return 0;
}

static int button_deconstrcut(Button *button)
{
	dbg_str(DBG_SUC,"button deconstruct,button addr:%p",button);

	return 0;
}

static int button_move(Button *button)
{
	dbg_str(DBG_SUC,"button move");
}

static int button_set(Button *button, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		button->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		button->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		button->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		button->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
		button->move = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(button->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"button set, not support %s setting",attrib);
	}

	return 0;
}

void *button_get(Button *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"button get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t button_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",button_set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",button_get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",button_construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",button_deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_FUNC_POINTER,"","move",button_move,sizeof(void *)},
	[6] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[7] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Button",button_class_info);

char *gen_button_setting_str()
{
    cjson_t *root,*b, *c, *e, *s;
    char *set_str;

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Button", b = cjson_create_object());{
            cjson_add_item_to_object(root, "Component", c = cjson_create_object());{
                cjson_add_item_to_object(root, "Container", e = cjson_create_object());{
                    cjson_add_item_to_object(e, "Subject", s = cjson_create_object());{
                        cjson_add_number_to_object(s, "x", 1);
                        cjson_add_number_to_object(s, "y", 25);
                        cjson_add_number_to_object(s, "width", 5);
                        cjson_add_number_to_object(s, "height", 125);
                    }
                    cjson_add_string_to_object(e, "name", "subject");
                }
                cjson_add_string_to_object(c, "name", "container");
            }
            cjson_add_string_to_object(c, "name", "component");
        }
        cjson_add_string_to_object(b, "name", "button");
    }
    set_str = cjson_print(root);

    return set_str;
}

void test_ui_button()
{
    Subject *subject;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    char buf[2048];

    set_str = gen_button_setting_str();

    subject = OBJECT_NEW(allocator, Button,set_str);

    object_dump(subject, "Button", buf, 2048);
    dbg_str(DBG_DETAIL,"Button dump: %s",buf);

    free(set_str);

}

