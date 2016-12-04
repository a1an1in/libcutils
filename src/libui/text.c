/**
 * @file text.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-03
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/text.h>

static int __construct(Text *text,char *init_str)
{
	dbg_str(OBJ_DETAIL,"text construct, text addr:%p",text);
    text->content = (String *)OBJECT_NEW(((Obj *)text)->allocator, String,NULL);
	if(text->content == NULL) {
		dbg_str(DBG_ERROR,"construct text error");
		return -1;
	}

	return 0;
}

static int __deconstrcut(Text *text)
{
	dbg_str(OBJ_DETAIL,"text deconstruct,text addr:%p",text);
    object_destroy(text->content);

	return 0;
}

static int __set(Text *text, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		text->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		text->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		text->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		text->deconstruct = value;
	} else if(strcmp(attrib, "load_text") == 0) {
		text->load_text = value;
	} else {
		dbg_str(OBJ_WARNNING,"text set,  \"%s\" setting is not support",attrib);
	}

	return 0;
}

static void * __get(Text *text, char *attrib)
{
    if(strcmp(attrib, "x") == 0){ 
    } else {
        dbg_str(OBJ_WARNNING,"text get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __load_text(Text *text,void *font)
{
	dbg_str(DBG_SUC,"SDL_Graph load text");
}

static class_info_entry_t text_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","load_text",__load_text,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_NORMAL_POINTER,"","String",NULL,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Text",text_class_info);

void test_obj_text()
{
	Text *text;
	allocator_t *allocator = allocator_get_default_alloc();

    text = OBJECT_NEW(allocator, Text,"");
	text->content->assign(text->content,"hello world");
}


