/**
 * @file painter.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/painter.h>

static int __construct(Painter *painter,char *init_str)
{
	dbg_str(DBG_SUC,"painter construct, painter addr:%p",painter);

	return 0;
}

static int __deconstrcut(Painter *painter)
{
	dbg_str(DBG_SUC,"painter deconstruct,painter addr:%p",painter);

	return 0;
}

static int __set(Painter *painter, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		painter->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		painter->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		painter->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		painter->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
		painter->move = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(painter->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"painter set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Painter *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"painter get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t painter_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[6] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Painter",painter_class_info);

void test_ui_painter()
{
    Painter *painter;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    cjson_t *root, *e, *s;
    char buf[2048];

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Painter", e = cjson_create_object());{
            cjson_add_string_to_object(e, "name", "alan");
        }
    }

    set_str = cjson_print(root);

    painter = OBJECT_NEW(allocator, Painter,set_str);

    object_dump(painter, "Painter", buf, 2048);
    dbg_str(DBG_DETAIL,"Painter dump: %s",buf);

    object_destroy(painter);

    free(set_str);

}


