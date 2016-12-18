#ifndef __TEXT_H__
#define __TEXT_H__

#include <libobject/obj.h>
#include <libobject/list.h>
#include <libui/graph.h>

typedef struct text_s Text;

typedef struct text_line_s{
	int head_offset;
	int tail_offset;
	int paragraph_num;
	int line_num_in_paragraph;
	int paragraph_line_num_in_text;
	int line_num;
	int line_lenth;
}text_line_t;

struct text_s{
	Obj obj;

	/*normal methods*/
	int (*construct)(Text *text,char *init_str);
	int (*deconstruct)(Text *text);
	int (*set)(Text *text, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods*/
	int (*parse_text)(Text *text, int offset, void *font);
	int (*get_head_offset_of_line)(Text *text, int offset);

	/*attribs*/
	char *content;
    List *line_info;
	int width;
};

#endif
