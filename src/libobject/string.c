/**
 * @file string.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/string.h>

static int __construct(String *string,char *init_str)
{
	dbg_str(OBJ_DETAIL,"string construct, string addr:%p",string);
	return 0;
}

static int __deconstrcut(String *string)
{
    dbg_str(OBJ_DETAIL,"string deconstruct,string addr:%p",string);
    if(string->value)
        allocator_mem_free(string->obj.allocator,string->value);

	return 0;
}

static int __set(String *string, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		string->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		string->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		string->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		string->deconstruct = value;
	} else if(strcmp(attrib, "assign") == 0) {
		string->assign = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(string->name,value,strlen(value));
	} else {
		dbg_str(OBJ_DETAIL,"string set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(String *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    }
    if(strcmp(attrib, "value") == 0) {
        return obj->value;
    } else {
        dbg_str(OBJ_WARNNING,"string get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static String *__assign(String *string,char *s)
{
    int len = strlen(s);

    if(string->value_max_len == 0) {
        string->value_max_len = 3 * len;
        string->value = (char *)allocator_mem_alloc(string->obj.allocator,string->value_max_len);
        if(string->value == NULL) {
            dbg_str(OBJ_WARNNING,"string assign alloc error");
            return string;
        }
    } else if( string->value_max_len < len) {
        allocator_mem_free(string->obj.allocator,string->value);

        string->value_max_len = 3 * len;
        string->value = (char *)allocator_mem_alloc(string->obj.allocator,string->value_max_len);
        if(string->value == NULL) {
            dbg_str(OBJ_WARNNING,"string assign alloc error");
            return string;
        }
    }

    memset(string->value,0, string->value_max_len);
    strncpy(string->value, s, len);
    string->value_len = len;

    return string;
}

static class_info_entry_t string_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_FUNC_POINTER,"","assign",__assign,sizeof(void *)},
	[6] = {ENTRY_TYPE_STRING,"char *","name",NULL,0},
	[7] = {ENTRY_TYPE_STRING,"char *","value",NULL,0},
	[8] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("String",string_class_info);

void test_obj_string()
{
    String *string;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    cjson_t *root, *e, *s;
    char buf[2048];

    dbg_str(DBG_DETAIL,"test_obj_string");
    root = cjson_create_object();{
        cjson_add_item_to_object(root, "String", e = cjson_create_object());{
            cjson_add_string_to_object(e, "name", "alan");
        }
    }

    set_str = cjson_print(root);

    string = OBJECT_NEW(allocator, String,set_str);

    string->assign(string,"hello world!");

    object_dump(string, "String", buf, 2048);
    dbg_str(OBJ_DETAIL,"String dump: %s",buf);

    free(set_str);

    object_destroy(string);

}


