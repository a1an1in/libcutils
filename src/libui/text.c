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
#include <libui/character.h>
#include <libobject/list_linked.h>

extern char *global_text;

int get_offset_at_dessignated_position_in_the_line(text_line_t *line, int pos, Font *font)
{
	Character *character;
    char c;
    int i;
    int w = 0;

    if(pos == 0) return 0;

    for( i = 0; i < line->tail - line->head + 1; i++) {
        c         = line->head[i];
        character = (Character *)font->ascii[c].character;
        w += character->width;
        if(w == pos){
            return i + 1;
        }
    }

    dbg_str(DBG_WARNNING,"pos may has problem, not find correct offset");

    return -1;
}
int extract_text_disturbed_by_inserting(Text *text, int line_num, int offset,  char *str, int len, Font *font)
{
	text_line_t *line_info = NULL;
    Iterator *cur          = NULL, *end;
    int find_flag          = -1, line_count = 0;
	int i                  = 0;

    cur         = text->line_info->begin(text->line_info);
    end         = text->line_info->end(text->line_info);


    for (i = 0; !end->equal(end,cur); cur->next(cur), i++) {
		if (i == line_num) {
            line_info = cur->get_vpointer(cur);
            /*
             *os = get_offset_at_dessignated_position_in_the_line(line_info, pos,font);
             */
            if (offset < 0) return -1;
            strncpy(str, line_info->head + offset, line_info->tail -line_info->head - offset + 1);
            line_count ++;
            find_flag = 1;
            dbg_str(DBG_DETAIL,"insert start from:%s", line_info->head + offset);
            if ((*line_info->tail) == '\n') {
				break;
			} else if(line_info->tail == '\0') {
				break;
			}
            continue;
		}
        if (find_flag == 1) { 
            line_count ++;
            line_info = cur->get_vpointer(cur);
            if (strlen(str) + line_info->tail - line_info->head + 1 > len) {
                dbg_str(DBG_WARNNING,"buffer too small, please check");
                return -1;
            }
			/*
             *dbg_str(DBG_DETAIL,"%s", line_info->head);
			 */
			strncpy(str + strlen(str), line_info->head, line_info->tail - line_info->head + 1);
            if ((*line_info->tail) == '\n') {
				break;
			}
        }
    }

    object_destroy(cur);
    object_destroy(end);

	return line_count;

}

int write_n_line(Text *text, int offset, void *font)
{
}

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
	} else if(strcmp(attrib, "write_text") == 0) {
		text->write_text = value;
	} else if(strcmp(attrib, "write_char") == 0) {
		text->write_char = value;
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

int __write_text(Text *text, int offset, void *font)
{
#define MAX_TEXT_LINE_LENTH 256
	int line_width                = text->width;
	int head_offset               = 0;
	int tail_offset               = 0;
	int line_num                  = 0;
	int line_lenth                = 0;
	int x                         = 0, y = 0;
	Font *f                       = (Font *)font;
	allocator_t *allocator        = ((Obj *)text)->allocator;
	int len, i, line_offset        = 0;
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
			line_offset            = -1;
			line_info.string      = OBJECT_NEW(allocator, String,NULL);
            line_info.string->pre_alloc(line_info.string, MAX_TEXT_LINE_LENTH);
		}

		line_offset++;

		if(x + c_witdh > line_width) {//line end
			line_info.line_lenth  = x;
			line_num++;
			line_info.head        = line_info.string->value;
			line_info.tail        = line_info.head + line_offset - 1;
			text->line_info->push_back(text->line_info, &line_info);

			x                     = 0;
			x                    += c_witdh;
			memset(&line_info, 0, sizeof(line_info));
			line_offset            = 0;
			line_info.string      = OBJECT_NEW(allocator, String,NULL);
            line_info.string->pre_alloc(line_info.string, MAX_TEXT_LINE_LENTH);
			line_info.string->append_char(line_info.string,c);

			if(c == '\n') {
				line_info.line_lenth  = x;
				line_num++;
				line_info.string->append_char(line_info.string,c);
				line_info.head        = line_info.string->value;
				line_info.tail        = line_info.head + line_offset;
				text->line_info->push_back(text->line_info, &line_info);
				x                     = 0;
			} else if( i == len - 1) {
				line_info.line_lenth  = x;
				text->total_line_num  = line_num;
				line_info.head        = line_info.string->value;
				line_info.tail        = line_info.head + line_offset;
				text->line_info->push_back(text->line_info, &line_info);
			}
		} else {
			x                    += c_witdh;
			line_info.string->append_char(line_info.string,c);

			if(c == '\n') {
				line_info.line_lenth  = x;
				line_num++;
				line_info.head        = line_info.string->value;
				line_info.tail        = line_info.head + line_offset;
				text->line_info->push_back(text->line_info, &line_info);
				x                     = 0;
			} else if( i == len - 1) {
				line_info.line_lenth  = x;
				text->total_line_num  = line_num;
				line_info.head        = line_info.string->value;
				line_info.tail        = line_info.head + line_offset;
				text->line_info->push_back(text->line_info, &line_info);
			}
		}

	}

	return 0;
#undef MAX_TEXT_LINE_LENTH
}

int *__write_char(Text *text,int line_num,  int offset, char c,void *font)
{
#define MAX_MODULATE_STR_LEN 1024
    Iterator *cur, *end;
	text_line_t *line_info;
	int i   = 0;
	int ret = -1;
    char str[MAX_MODULATE_STR_LEN] = {0};
    int line_count;

#if 0
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
#endif
    line_count = extract_text_disturbed_by_inserting(text, line_num, offset, str, MAX_MODULATE_STR_LEN, font);
    dbg_str(DBG_DETAIL,"text_line_disturbed_by_inserting:%s",str);

	return ret;
#undef MAX_MODULATE_STR_LEN
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
    [5 ] = {ENTRY_TYPE_FUNC_POINTER,"","write_text",__write_text,sizeof(void *)},
    [6 ] = {ENTRY_TYPE_FUNC_POINTER,"","write_char",__write_char,sizeof(void *)},
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

    dbg_str(DBG_DETAIL,"head=%p,tail =%p,data =%s", 
            line_info->head, line_info->tail, line_info->head);
}

void test_obj_text()
{
	Text *text;
	allocator_t *allocator = allocator_get_default_alloc();

	dbg_str(DBG_DETAIL,"test_obj_text");
    text = OBJECT_NEW(allocator, Text,"");

}


