/**
 * @file component.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/component.h>

static int __construct(Component *component,char *init_str)
{
    dbg_str(DBG_DETAIL,"component construct, component addr:%p",component);

    return 0;
}

static int __deconstrcut(Component *component)
{
    dbg_str(DBG_DETAIL,"component deconstruct,component addr:%p",component);

    return 0;
}

static int __move(Component *component)
{
    dbg_str(DBG_DETAIL,"component move");
}

static int __set(Component *component, char *attrib, void *value)
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
    } else if(strcmp(attrib, "draw") == 0) {
        component->draw = value;
    } else if(strcmp(attrib, "load_resources") == 0) {
        component->load_resources = value;
    } else if(strcmp(attrib, "text_key_input") == 0) {
        component->text_key_input = value;
    } else if(strcmp(attrib, "backspace_key_input") == 0) {
        component->backspace_key_input = value;
    } else if(strcmp(attrib, "up_key_down") == 0) {
        component->up_key_down = value;
    } else if(strcmp(attrib, "down_key_down") == 0) {
        component->down_key_down = value;
    } else if(strcmp(attrib, "left_key_down") == 0) {
        component->left_key_down = value;
    } else if(strcmp(attrib, "right_key_down") == 0) {
        component->right_key_down = value;
    } else if(strcmp(attrib, "pageup_key_down") == 0) {
        component->pageup_key_down = value;
    } else if(strcmp(attrib, "pagedown_key_down") == 0) {
        component->pagedown_key_down = value;
    } else if(strcmp(attrib, "one_line_up") == 0) {
        component->one_line_up = value;
    } else if(strcmp(attrib, "one_line_down") == 0) {
        component->one_line_down = value;
    } else if(strcmp(attrib, "name") == 0) {
        strncpy(component->name,value,strlen(value));
    } else {
        dbg_str(DBG_DETAIL,"component set, not support %s setting",attrib);
    }

    return 0;
}

static void *__get(Component *obj, char *attrib)
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
    [0 ] = {ENTRY_TYPE_OBJ,"Container","container",NULL,sizeof(void *)},
    [1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
    [2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
    [3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
    [4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
    [5 ] = {ENTRY_TYPE_FUNC_POINTER,"","move",__move,sizeof(void *)},
    [6 ] = {ENTRY_TYPE_VFUNC_POINTER,"","draw",NULL,sizeof(void *)},
    [7 ] = {ENTRY_TYPE_VFUNC_POINTER,"","load_resources",NULL,sizeof(void *)},
    [8 ] = {ENTRY_TYPE_VFUNC_POINTER,"","text_key_input",NULL,sizeof(void *)},
    [9 ] = {ENTRY_TYPE_VFUNC_POINTER,"","backspace_key_input",NULL,sizeof(void *)},
    [10] = {ENTRY_TYPE_VFUNC_POINTER,"","up_key_down",NULL,sizeof(void *)},
    [11] = {ENTRY_TYPE_VFUNC_POINTER,"","down_key_down",NULL,sizeof(void *)},
    [12] = {ENTRY_TYPE_VFUNC_POINTER,"","left_key_down",NULL,sizeof(void *)},
    [13] = {ENTRY_TYPE_VFUNC_POINTER,"","right_key_down",NULL,sizeof(void *)},
    [14] = {ENTRY_TYPE_VFUNC_POINTER,"","pageup_key_down",NULL,sizeof(void *)},
    [15] = {ENTRY_TYPE_VFUNC_POINTER,"","pagedown_key_down",NULL,sizeof(void *)},
    [16] = {ENTRY_TYPE_VFUNC_POINTER,"","one_line_up",NULL,sizeof(void *)},
    [17] = {ENTRY_TYPE_VFUNC_POINTER,"","one_line_down",NULL,sizeof(void *)},
    [18] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
    [19] = {ENTRY_TYPE_END},

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


