#ifndef __SDL_WINDOW_H__
#define __SDL_WINDOW_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/component.h>
#include <libui/graph.h>
#include <libui/image.h>
#include <libui/window.h>

typedef struct sdl_window_s SDL_Win;

struct sdl_window_s{
	Window window;

	int (*construct)(Window *window,char *init_str);
	int (*deconstruct)(Window *window);
	int (*set)(Window *window, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);
};

#endif
