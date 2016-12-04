#ifndef __TEXT_SDL_H__
#define __TEXT_SDL_H__

#include <libobject/obj.h>
#include <libobject/string.h>
#include <libui/event.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_events.h>

typedef struct sdl_ui_event_s _SDL_Event;

struct sdl_event_s{
	Event event;

	/*normal methods*/
	int (*construct)(Event *event,char *init_str);
	int (*deconstruct)(Event *event);
	int (*set)(Event *event, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods*/

	/*attribs*/
	SDL_Event ev;
};

#endif