#ifndef __FONT_H__
#define __FONT_H__

#include <libobject/obj.h>
#include <libobject/string.h>
/*
 *#include <libui/graph.h>
 */

typedef struct font_s Font;
struct font_s{
	Obj obj;

	/*normal methods*/
	int (*construct)(Font *font,char *init_str);
	int (*deconstruct)(Font *font);
	int (*set)(Font *font, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods*/
	int (*load_font)(Font *font);

	/*attribs*/
	String *path;
};

#endif
