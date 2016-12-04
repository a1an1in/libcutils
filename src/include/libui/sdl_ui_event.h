#ifndef __TEXT_SDL_H__
#define __TEXT_SDL_H__

#include <libobject/obj.h>
#include <libobject/string.h>
#include <libui/ui_event.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct sdl_ui_event_s SDL_UI_Event;

struct sdl_event_s{
	UI_Event event;

	/*normal methods*/
	int (*construct)(UI_Event *event,char *init_str);
	int (*deconstruct)(UI_Event *event);
	int (*set)(UI_Event *event, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods*/

	/*attribs*/
};

#endif
