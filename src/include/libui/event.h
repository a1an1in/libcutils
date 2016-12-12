#ifndef __UI_EVENT_H__
#define __UI_EVENT_H__

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
    int (*poll_event)(Event *event,void *window);

	/*attribs*/
};

#endif
