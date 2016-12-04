#ifndef __TEXT_H__
#define __TEXT_H__

#include <libobject/obj.h>
#include <libobject/string.h>
#include <libui/graph.h>

typedef struct text_s Text;
struct text_s{
	Obj obj;

	/*normal methods*/
	int (*construct)(Text *text,char *init_str);
	int (*deconstruct)(Text *text);
	int (*set)(Text *text, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods*/
	int (*load_text)(Text *text,void *font);

	/*attribs*/
	String *content;
};

#endif
