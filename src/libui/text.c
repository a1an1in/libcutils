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
#include <libobject/list_linked.h>

extern char *global_text;

static int __construct(Text *text,char *init_str)
{
    allocator_t *allocator = ((Obj *)text)->allocator;
	char *set_str          = "{\"Linked_List\":{\"List\":{ \"value_size\":%d}}}";
#define MAX_BUF_LEN 1024
	char buf[MAX_BUF_LEN];

	dbg_str(OBJ_DETAIL,"text construct, text addr:%p",text);
	snprintf(buf, MAX_BUF_LEN, set_str, sizeof(text_line_t));

	text->line_info  = OBJECT_NEW(allocator, Linked_List,buf);
	if(!text->line_info) {
		dbg_str(DBG_WARNNING, "create line_info err");
		return -1;
	}

	/*
	 *text->content = global_text;
	 */
	text->width   = 600;
    object_dump(text->line_info, "Linked_List", buf, MAX_BUF_LEN);
    dbg_str(DBG_DETAIL,"line info dump: %s",buf);

#undef MAX_BUF_LEN
	return 0;
}

static int __deconstrcut(Text *text)
{
	dbg_str(OBJ_DETAIL,"text deconstruct,text addr:%p",text);
	if(text->line_info)
		object_destroy(text->line_info);

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
	} else if(strcmp(attrib, "parse_text") == 0) {
		text->parse_text = value;
	} else if(strcmp(attrib, "get_head_offset_of_line") == 0) {
		text->get_head_offset_of_line = value;
	} else if(strcmp(attrib, "get_line_count") == 0) {
		text->get_line_count = value;
	} else if(strcmp(attrib, "get_text_line_info") == 0) {
		text->get_text_line_info = value;
	}
    else {
		dbg_str(OBJ_WARNNING,"text set,  \"%s\" setting is not support",attrib);
	}

	return 0;
}

static void * __get(Text *text, char *attrib)
{
    if(strcmp(attrib, "") == 0){ 
    } else {
        dbg_str(OBJ_WARNNING,"text get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

int __parse_text(Text *text, int offset, void *font)
{
	int line_width                = text->width;
	int head_offset               = 0;
	int tail_offset               = 0;
	int line_num                  = 0;
	int line_lenth                = 0;
	int x                         = 0, y = 0;
	Font *f                       = (Font *)font;
	allocator_t *allocator        = ((Obj *)text)->allocator;
	int len, i, line_count        = 0;
	text_line_t line_info;
	char c;
	int c_witdh, c_height;

	memset(&line_info, 0, sizeof(line_info));
	len  = strlen(text->content + offset);

	for(i = 0; i < len; i++) {
		c       = text->content[offset + i];
		c_witdh = f->get_character_width(f,c);
		if(x == 0) {
			memset(&line_info, 0, sizeof(line_info));
			line_count            = -1;
			line_info.string      = OBJECT_NEW(allocator, String,NULL);
		}

		line_count++;

		if(c == '\n') {
			line_info.line_lenth  = x;
			line_info.line_num    = line_num++;
			line_info.string->append_char(line_info.string,c);
			line_info.head        = line_info.string->value;
			line_info.tail        = line_info.head + line_count;
			text->line_info->push_back(text->line_info, &line_info);
			x                     = 0;

		} else if(x + c_witdh > line_width) {//line end
			line_info.line_lenth  = x;
			line_info.line_num    = line_num++;
			line_info.head        = line_info.string->value;
			line_info.tail        = line_info.head + line_count;
			text->line_info->push_back(text->line_info, &line_info);

			x                     = 0;
			x                    += c_witdh;
			memset(&line_info, 0, sizeof(line_info));
			line_count            = 0;
			line_info.string      = OBJECT_NEW(allocator, String,NULL);
			line_info.string->append_char(line_info.string,c);

		} else if(i == len - 1) {
			x                    += c_witdh;
			line_info.line_lenth  = x;
			line_info.line_num    = line_num;
			text->total_line_num  = line_num;
			line_info.string->append_char(line_info.string,c);
			line_info.head        = line_info.string->value;
			line_info.tail        = line_info.head + line_count;
			text->line_info->push_back(text->line_info, &line_info);

		} else {
			x                    += c_witdh;
			line_info.string->append_char(line_info.string,c);
		}

	}

	return 0;
}

int __get_head_offset_of_line(Text *text, int line_num)
{
    Iterator *cur, *end;
	text_line_t *line_info;
	int i   = 0;
	int ret = -1;

    cur = text->line_info->begin(text->line_info);
    end = text->line_info->end(text->line_info);

    for(i = 0; !end->equal(end,cur); cur->next(cur), i++) {
		if(i == line_num) {
			line_info = cur->get_vpointer(cur);
			ret       = line_info->head_offset;
			break;
		}
    }

    object_destroy(cur);
    object_destroy(end);

	return ret;
}

void *__get_text_line_info(Text *text, int line_num)
{
    Iterator *cur, *end;
	text_line_t *line_info = NULL;
	int i   = 0;

    cur = text->line_info->begin(text->line_info);
    end = text->line_info->end(text->line_info);

    for(i = 0; !end->equal(end,cur); cur->next(cur), i++) {
		if(i == line_num) {
			line_info = cur->get_vpointer(cur);
			break;
		}
    }

    object_destroy(cur);
    object_destroy(end);

	return line_info;
}
int __get_line_count(Text *text)
{
	return text->total_line_num;
}

static class_info_entry_t text_class_info[] = {
    [0 ] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
    [1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
    [2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
    [3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
    [4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
    [5 ] = {ENTRY_TYPE_FUNC_POINTER,"","parse_text",__parse_text,sizeof(void *)},
    [6 ] = {ENTRY_TYPE_FUNC_POINTER,"","get_head_offset_of_line",__get_head_offset_of_line,sizeof(void *)},
    [7 ] = {ENTRY_TYPE_FUNC_POINTER,"","get_line_count",__get_line_count,sizeof(void *)},
    [8 ] = {ENTRY_TYPE_FUNC_POINTER,"","get_text_line_info",__get_text_line_info,sizeof(void *)},
    [9 ] = {ENTRY_TYPE_NORMAL_POINTER,"","List",NULL,sizeof(void *)},
    [10] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Text",text_class_info);

void print_line_info(Iterator *iter)
{
    LList_Iterator *i      = (LList_Iterator *)iter;
	text_line_t *line_info = i->get_vpointer(iter);

    dbg_str(DBG_DETAIL,"head=%p,tail =%p,line_num =%d,data =%s", 
            line_info->head, line_info->tail, line_info->line_num, line_info->head);
}

void test_obj_text()
{
	Text *text;
	allocator_t *allocator = allocator_get_default_alloc();

	dbg_str(DBG_DETAIL,"test_obj_text");
    text = OBJECT_NEW(allocator, Text,"");

}


