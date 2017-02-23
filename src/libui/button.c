/**
 * @file button.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
/* Copyright (c) 2015-2020 alan lin <a1an1in@sina.com>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <libui/button.h>
#include <libui/border_layout.h>
#include <libui/sdl_window.h>
#include <libui/label.h>

static int __construct(Button *button,char *init_str)
{
    Container *container = (Container *)button;
    Subject *subject     = (Subject *)button;
    char buf[2048];

	dbg_str(DBG_SUC,"button construct, button addr:%p",button);

    gen_label_setting_str(subject->x, subject->y, subject->width, subject->height, 
                          "label", (void *)buf);
    subject   = OBJECT_NEW(((Obj *)button)->allocator, Label,buf);

    container->add_component((Container *)button, NULL, subject);

	return 0;
}

static int __deconstrcut(Button *button)
{
	dbg_str(DBG_SUC,"button deconstruct,button addr:%p",button);
    //..........
    //release label

	return 0;
}

static int __set(Button *button, char *attrib, void *value)
{
	if (strcmp(attrib, "set") == 0) {
		button->set = value;
    } else if (strcmp(attrib, "get") == 0) {
		button->get = value;
	} else if (strcmp(attrib, "construct") == 0) {
		button->construct = value;
	} else if (strcmp(attrib, "deconstruct") == 0) {
		button->deconstruct = value;
	} 
    else if (strcmp(attrib, "move") == 0) {
		button->move = value;
    } else if (strcmp(attrib, "mouse_button_down") == 0) {
        button->mouse_button_down = value;
    } else if (strcmp(attrib, "mouse_over") == 0) {
        button->mouse_over = value;
	} 
    else {
		dbg_str(DBG_DETAIL,"button set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Button *obj, char *attrib)
{
    if (strcmp(attrib, "") == 0) {
    } else {
        dbg_str(DBG_WARNNING,"button get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static void __mouse_button_down(Component *component,void *event, void *window) 
{
    __Event *e = (__Event *)event;

    dbg_str(DBG_DETAIL,
            "%s process mouse_button_down event: Mouse button %d pressed at %d,"
            "%d with click count %d in window %d",
            component->name, e->button, e->x, e->y, e->clicks, e->windowid); 
}

static void __mouse_over(Component *component,void *event, void *window) 
{
    __Event *e           = (__Event *)event;

    /*
     *dbg_str(DBG_DETAIL, "EVENT: Mouse: moved to %d,%d (%d,%d) in window %d",
     *        e->x, e->y, e->xrel, e->yrel, e->windowid);
     */
}

static class_info_entry_t button_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_FUNC_POINTER,"","move",NULL,sizeof(void *)},
    [6] = {ENTRY_TYPE_FUNC_POINTER,"","mouse_button_down",__mouse_button_down,sizeof(void *)},
    [7] = {ENTRY_TYPE_FUNC_POINTER,"","mouse_over",__mouse_over,sizeof(void *)},
	[8] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Button",button_class_info);

static void gen_button_setting_str(int x, int y, int width, int height, char *name, void *out)
{
    char *set_str;

    set_str = "{\
                    \"Subject\": {\
                        \"x\":%d,\
                        \"y\":%d,\
                        \"width\":%d,\
                        \"height\":%d\
                    },\
                    \"Container\": {\
                        \"map_type\":1\
                    },\
                    \"Component\": {\
                        \"name\": \"%s\"\
                    }\
               }";

    sprintf(out, set_str, x, y, width, height, name);

    return ;
}

static void *new_button(allocator_t *allocator, int x, int y, int width, int height, char *name)
{
    Subject *subject;
    char *set_str;
    char buf[2048];

    gen_button_setting_str(x, y, width, height, name, (void *)buf);
    subject   = OBJECT_NEW(allocator, Button,buf);

    object_dump(subject, "Button", buf, 2048);
    /*
     *dbg_str(DBG_DETAIL,"Button dump: %s",buf);
     */

    return subject;
}


void test_ui_button()
{
    allocator_t *allocator = allocator_get_default_alloc();
    Window *window;
    Border_Layout *layout;
    Button *button;
    char *set_str;
    char buf[2048];

    set_str = gen_window_setting_str();
    window  = OBJECT_NEW(allocator, Sdl_Window,set_str);

    /*
     *object_dump(window, "Sdl_Window", buf, 2048);
     *dbg_str(DBG_DETAIL,"Window dump: %s",buf);
     */

    layout = new_border_layout(allocator, 0, 0, 600, 600, "border layout");

    button = new_button(allocator,0, 0, 100, 50, "button02");
    layout->add_component((Container *)layout, "Center", button);

    window->add_component((Container *)window, NULL, layout);
    window->load_resources(window);
    window->update_window(window);
    window->event->poll_event(window->event, window);

    object_destroy(window);
}

