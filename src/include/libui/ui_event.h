#ifndef __TEXT_H__
#define __TEXT_H__

#include <libobject/obj.h>
#include <libobject/string.h>
#include <libui/graph.h>

typedef struct ui_event_s UI_Event;
struct ui_event_s{
	Obj obj;

	/*normal methods*/
	int (*construct)(UI_Event *event,char *init_str);
	int (*deconstruct)(UI_Event *event);
	int (*set)(UI_Event *event, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods*/

	/*attribs*/
};

#endif
