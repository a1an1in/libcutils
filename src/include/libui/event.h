#ifndef __TEXT_H__
#define __TEXT_H__

#include <libobject/obj.h>
#include <libobject/string.h>
#include <libui/graph.h>

typedef struct ui_event_s Event;
struct ui_event_s{
	Obj obj;

	/*normal methods*/
	int (*construct)(Event *event,char *init_str);
	int (*deconstruct)(Event *event);
	int (*set)(Event *event, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods*/

	/*attribs*/
};

#endif
