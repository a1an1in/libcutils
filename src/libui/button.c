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

static void gen_label_setting_str(int x, int y, int width, int height, char *name, void *out)
{
    char *set_str;

    set_str = "{\
                    \"Subject\": {\
                        \"x\":%d,\
                        \"y\":%d,\
                        \"width\":%d,\
                        \"height\":%d\
                    },\
                    \"Component\": {\
                        \"name\": \"%s\"\
                    },\
                    \"Label\": {\
                        \"text_overflow_flag\": 0\
                    }\
                }";

    sprintf(out, set_str, x, y, width, height, name);

    return ;
}

static void *new_label(allocator_t *allocator, int x, int y, int width, int height, char *name)
{
    Subject *subject;
    char *set_str;
    char buf[2048];

    gen_label_setting_str(x, y, width, height, name, (void *)buf);
    subject   = OBJECT_NEW(allocator, Label,buf);

    object_dump(subject, "Label", buf, 2048);

    return subject;
}

static int __construct(Button *button,char *init_str)
{
    Container *container = (Container *)button;
    Subject *subject = (Subject *)button;
    char buf[2048];

	dbg_str(DBG_SUC,"button construct, button addr:%p",button);

    gen_label_setting_str(subject->x, subject->y, subject->width, subject->height, 
                          ((Component *)button)->name, (void *)buf);
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
	} else if (strcmp(attrib, "move") == 0) {
		button->move = value;
	} else {
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

static class_info_entry_t button_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_FUNC_POINTER,"","move",NULL,sizeof(void *)},
	[6] = {ENTRY_TYPE_END},

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

static char *gen_border_layout_setting_str(int x, int y, int width, int height, char *name, void *out)
{
    char *set_str = NULL;

    set_str = "{\
                    \"Subject\": {\
                        \"x\":%d,\
                        \"y\":%d,\
                        \"width\":%d,\
                        \"height\":%d\
                    },\
                    \"Container\": {\
                        \"map_type\":%d\
                    },\
                    \"Component\": {\
                        \"name\":\"%s\"\
                    },\
                    \"Border_Layout\":{\
                        \"hgap\":%d,\
                        \"vgap\":%d\
                    }\
                }";

    sprintf(out, set_str, x, y, width, height,1, name, 4,4, 5, 2);

    return out;
}

static void *new_border_layout(allocator_t *allocator, int x, int y, int width, int height, char *name)
{
    char *set_str;
    char buf[2048];
    Container *container;

    gen_border_layout_setting_str(x, y, width, height, name, buf);
    container = OBJECT_NEW(allocator, Border_Layout,buf);

    object_dump(container, "Border_Layout", buf, 2048);
    dbg_str(DBG_DETAIL,"Border_Layout dump: %s",buf);

    return container;
}

void test_ui_button()
{
    allocator_t *allocator = allocator_get_default_alloc();
    Window *window;
    Border_Layout *layout;
    Button *l;
    char *set_str;
    char buf[2048];

    set_str = gen_window_setting_str();
    window  = OBJECT_NEW(allocator, Sdl_Window,set_str);

    object_dump(window, "Sdl_Window", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

    layout = new_border_layout(allocator, 0, 0, 600, 600, "layout");

    l = new_button(allocator,0, 0, 100, 50, "button02");
    layout->add_component((Container *)layout, "Center", l);

    window->add_component((Container *)window, NULL, layout);
    window->load_resources(window);
    window->update_window(window);
    window->event->poll_event(window->event, window);

    object_destroy(window);
}

