/**
 * @file girdlayout.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 
 * @date 2017-02-09
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
#include <libui/border_layout.h>
#include <libui/sdl_window.h>
#include <libui/character.h>
#include <libui/timer.h>
#include <miscellany/buffer.h>
#include <libui/label.h>

static int __construct(Border_Layout *border_layout,char *init_str)
{
    allocator_t *allocator = ((Obj *)border_layout)->allocator;
    Border_Layout *l       = border_layout;
    uint32_t i;

    dbg_str(DBG_DETAIL,"border_layout construct");

    memset(l->blocks, 0, sizeof(layout_block_t) * BORDER_LAYOUT_MAX); 

    l->layout_width_default                = 600;
    l->layout_height_default               = 600;
    l->layout_width                        = l->layout_width_default;
    l->layout_height                       = l->layout_height_default;

    l->blocks[BORDER_LAYOUT_NORTH].width   = l->layout_width_default;
    l->blocks[BORDER_LAYOUT_NORTH].height  = l->layout_height / 3;
    l->blocks[BORDER_LAYOUT_WEST].width    = l->layout_width_default / 3;
    l->blocks[BORDER_LAYOUT_WEST].height   = l->layout_height / 3;
    l->blocks[BORDER_LAYOUT_CENTER].width  = l->layout_width_default / 3;
    l->blocks[BORDER_LAYOUT_CENTER].height = l->layout_height / 3;
    l->blocks[BORDER_LAYOUT_EAST].width    = l->layout_width_default / 3;
    l->blocks[BORDER_LAYOUT_EAST].height   = l->layout_height / 3;
    l->blocks[BORDER_LAYOUT_SOUTH].width   = l->layout_width_default;
    l->blocks[BORDER_LAYOUT_SOUTH].height  = l->layout_height / 3;

    dbg_str(DBG_DETAIL,"border_layout construct end");

    return 0;
}

static int __deconstrcut(Border_Layout *border_layout)
{
    allocator_t *allocator = ((Obj *)border_layout)->allocator;

    dbg_str(DBG_SUC,"border_layout deconstruct");

    return 0;
}

static int __set(Border_Layout *border_layout, char *attrib, void *value)
{
    if (strcmp(attrib, "set") == 0) {
        border_layout->set = value;
    } else if (strcmp(attrib, "get") == 0) {
        border_layout->get = value;
    } else if (strcmp(attrib, "construct") == 0) {
        border_layout->construct = value;
    } else if (strcmp(attrib, "deconstruct") == 0) {
        border_layout->deconstruct = value;
    }
    /*vitual methods*/
    else if (strcmp(attrib, "add_component") == 0) {
        border_layout->add_component = value;
    } else if (strcmp(attrib, "draw") == 0) {
        border_layout->draw = value;
    } else if (strcmp(attrib, "load_resources") == 0) {
        border_layout->load_resources = value;
    }
    /*attribs*/
    else if (strcmp(attrib, "name") == 0) {
        dbg_str(DBG_SUC,"set border_layout name");
        strncpy(border_layout->name,value,strlen(value));
    } else if (strcmp(attrib, "hgap") == 0) {
        border_layout->hgap = *((uint32_t *)value);
    } else if (strcmp(attrib, "vgap") == 0) {
        border_layout->vgap = *((uint32_t *)value);
    } else {
        dbg_str(DBG_DETAIL,"border_layout set, not support %s setting",attrib);
    }

    return 0;
}

static void *__get(Border_Layout *obj, char *attrib)
{
    if (strcmp(attrib, "name") == 0) {
        return obj->name;
    } else if (strcmp(attrib, "hgap") == 0) {
        return &obj->hgap;
    } else if (strcmp(attrib, "vgap") == 0) {
        return &obj->vgap;
    } else {
        dbg_str(DBG_WARNNING,"border_layout get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __add_component(Container *obj, void *pos, void *component)
{
    Border_Layout *l     = (Border_Layout *)obj;
    Container *container = (Container *)obj;
    Map *map             = container->map;
    char buffer[8]       = {0};
    Component *c         = (Component *)component;
    Subject *subject     = (Subject *)c;
    uint8_t rearrange_comonents_flag = 0;
    position_t position;

    if (pos == NULL) {
        dbg_str(DBG_DETAIL,"border layout add component error, please dessignate pos");
        return -1;
    }

    if (strncmp(pos, "North", 5) == 0) {
        l->blocks[BORDER_LAYOUT_NORTH].component = component;
        position.x = 0;
        position.y = 0;
    } else if (strncmp(pos, "West", 4) == 0) {
        l->blocks[BORDER_LAYOUT_WEST].component = component;
        position.x = 0;
        position.y = l->blocks[BORDER_LAYOUT_NORTH].height + l->vgap;
    } else if (strncmp(pos, "Center", 6) == 0) {
        l->blocks[BORDER_LAYOUT_CENTER].component = component;
        position.x = l->blocks[BORDER_LAYOUT_WEST].width + l->hgap;
        position.y = l->blocks[BORDER_LAYOUT_NORTH].height + l->vgap;
    } else if (strncmp(pos, "East", 4) ==0) {
        l->blocks[BORDER_LAYOUT_EAST].component = component;
        position.x = l->blocks[BORDER_LAYOUT_WEST].width + l->hgap +
                     l->blocks[BORDER_LAYOUT_CENTER].width + l->hgap;
        position.y = l->blocks[BORDER_LAYOUT_NORTH].height + l->vgap;
    } else if (strncmp(pos, "South", 5) == 0) {
        l->blocks[BORDER_LAYOUT_SOUTH].component = component;
        position.x = 0;
        position.y = l->blocks[BORDER_LAYOUT_NORTH].height + l->vgap +
                     l->blocks[BORDER_LAYOUT_WEST].height + l->vgap;
    } else {
        dbg_str(DBG_WARNNING,"borderlayout add component, pos par err");
        return -1;
    }

    dbg_str(DBG_SUC,"position x=%d, y=%d", position.x, position.y);
    container->update_component_position(c, &position);

    addr_to_buffer(c,(uint8_t *)buffer);
    map->insert(map, c->name, buffer);

    return 0;
}

static void draw_subcomponent(Iterator *iter, void *arg) 
{
    Component *component;
    uint8_t *addr;
    Graph *g = (Graph *)arg;

    addr      = (uint8_t *)iter->get_vpointer(iter);
    component = (Component *)buffer_to_addr(addr);

    if (component->draw) component->draw(component, g);
}

static void draw_border(Component *component, void *graph) 
{
    Border_Layout *l = (Border_Layout *)component;
    Graph *g         = (Graph *)graph;
    int i;
    position_t start, end;

    dbg_str(DBG_SUC,"draw_border");

}

/*reimplement the virtual func draw() int Component class*/
static int __draw(Component *component, void *graph)
{
    Container *container = (Container *)component;
    Graph *g             = (Graph *)graph;

    dbg_str(DBG_SUC,"%s draw", ((Obj *)component)->name);

    /*draw layouts*/
    draw_border(component, graph);

    /*draw subcomponent*/
    container->for_each_component(container, draw_subcomponent, g);
}

static class_info_entry_t border_layout_class_info[] = {
    [0 ] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
    [1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
    [2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
    [3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
    [4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
    [5 ] = {ENTRY_TYPE_FUNC_POINTER,"","add_component",__add_component,sizeof(void *)},
    [6 ] = {ENTRY_TYPE_FUNC_POINTER,"","draw",__draw,sizeof(void *)},
    [7 ] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
    [8 ] = {ENTRY_TYPE_INT32_T,"int","hgap",NULL,sizeof(int)},
    [9 ] = {ENTRY_TYPE_INT32_T,"int","vgap",NULL,sizeof(int)},
    [10] = {ENTRY_TYPE_END},
};
REGISTER_CLASS("Border_Layout",border_layout_class_info);

#if 1
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
    /*
     *dbg_str(DBG_DETAIL,"Label dump: %s",buf);
     */

    return subject;
}
#endif

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

void test_ui_border_layout()
{
    allocator_t *allocator = allocator_get_default_alloc();
    Window *window;
    Border_Layout *layout;
    Label *l;
    char *set_str;
    char buf[2048];

    set_str = gen_window_setting_str();
    window  = OBJECT_NEW(allocator, Sdl_Window,set_str);

    object_dump(window, "Sdl_Window", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

    layout = new_border_layout(allocator, 0, 0, 600, 600, "layout");

    l = new_label(allocator,0, 0, 80, 20, "label00");
    layout->add_component((Container *)layout, "North", l);
    l = new_label(allocator,0, 0, 80, 20, "label01");
    layout->add_component((Container *)layout, "West", l);
    l = new_label(allocator,0, 0, 80, 20, "label02");
    layout->add_component((Container *)layout, "Center", l);
    l = new_label(allocator,0, 0, 80, 20, "label03");
    layout->add_component((Container *)layout, "East", l);
    l = new_label(allocator,0, 0, 80, 20, "label10");
    layout->add_component((Container *)layout, "South", l);

    window->add_component((Container *)window, NULL, layout);
    window->load_resources(window);
    window->update_window(window);
    window->event->poll_event(window->event, window);

    object_destroy(window);
}
