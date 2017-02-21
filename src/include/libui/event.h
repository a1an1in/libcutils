#ifndef __UI_EVENT_H__
#define __UI_EVENT_H__

#include <libobject/obj.h>
#include <libobject/string.h>
#include <libui/graph.h>

typedef struct ui_event_s __Event;
struct ui_event_s{
	Obj obj;

	/*normal methods*/
	int (*construct)(__Event *event,char *init_str);
	int (*deconstruct)(__Event *event);
	int (*set)(__Event *event, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods*/
    int (*poll_event)(__Event *event,void *window);
    void (*mouse_button_down)(int x, int y, int button, int clicks, int windowid, void *window);
    void (*mouse_motion)(int x, int y, int xrel, int yrel, int windowid, void *window);
    void (*mouse_wheel)(int x, int y, int direction, int windowid, void *window);
    void (*window_moved)(void *window);
    void (*window_resized)(int data1, int data2, int windowid, void *window);

	/*attribs*/
};

#endif
