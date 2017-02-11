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
#include <libui/gridlayout.h>
#include <libui/sdl_window.h>
#include <libui/character.h>
#include <libui/timer.h>
#include <miscellany/buffer.h>
#include <libui/label.h>

static int __construct(Gridlayout *gridlayout,char *init_str)
{
    allocator_t *allocator = ((Obj *)gridlayout)->allocator;
    Gridlayout *l = gridlayout;
    uint32_t i;

    dbg_str(DBG_DETAIL,"gridlayout construct");

    l->default_unit_width  = 100;
    l->default_unit_height = 20;
    l->cur_col             = 0;
    l->cur_row             = 0;

    if (l->row_max == 0) { l->row_max = 1;}
    if (l->col_max == 0) { l->col_max = 1;}

    dbg_str(DBG_DETAIL,"row_max = %d, col_max=%d", l->row_max, l->col_max);

    /*alloc grid height and width arrays*/
    l->row_height = (uint32_t *) 
                    allocator_mem_alloc(allocator,
                                        l->row_max * sizeof(uint32_t *));
    l->col_width  = (uint32_t *)
                    allocator_mem_alloc(allocator,
                                        l->col_max * sizeof(uint32_t *));
    /*init grid row height*/
    for (i = 0; i < l->row_max; i++) {
       l->row_height[i] = l->default_unit_height; 
    }

    /*init grid col width*/
    for ( i = 0; i < l->col_max; i++) {
        l->col_width[i] = l->default_unit_width;
    }

    /*alloc space for grid componets*/
    l->grid_components = (Component **) 
                         allocator_mem_alloc(allocator,
                                             l->row_max * l->col_max * sizeof(Component *));

    memset(l->grid_components, 0, l->row_max * l->col_max * sizeof(Component *));

    return 0;
}

static int __deconstrcut(Gridlayout *gridlayout)
{
    allocator_t *allocator = ((Obj *)gridlayout)->allocator;

    dbg_str(DBG_SUC,"gridlayout deconstruct");

    allocator_mem_free(allocator, gridlayout->row_height);
    allocator_mem_free(allocator, gridlayout->col_width);

    return 0;
}

static int __set(Gridlayout *gridlayout, char *attrib, void *value)
{
    if (strcmp(attrib, "set") == 0) {
        gridlayout->set = value;
    } else if (strcmp(attrib, "get") == 0) {
        gridlayout->get = value;
    } else if (strcmp(attrib, "construct") == 0) {
        gridlayout->construct = value;
    } else if (strcmp(attrib, "deconstruct") == 0) {
        gridlayout->deconstruct = value;
    }
    /*vitual methods*/
    else if (strcmp(attrib, "add_component") == 0) {
        gridlayout->add_component = value;
    } else if (strcmp(attrib, "draw") == 0) {
        gridlayout->draw = value;
    } else if (strcmp(attrib, "load_resources") == 0) {
        gridlayout->load_resources = value;
    }
    /*attribs*/
    else if (strcmp(attrib, "name") == 0) {
        dbg_str(DBG_SUC,"set gridlayout name");
        strncpy(gridlayout->name,value,strlen(value));
    } else if(strcmp(attrib, "row_max") == 0) {
        gridlayout->row_max = *((uint32_t *)value);
    } else if(strcmp(attrib, "col_max") == 0) {
        gridlayout->col_max = *((uint32_t *)value);
    } else {
        dbg_str(DBG_DETAIL,"gridlayout set, not support %s setting",attrib);
    }

    return 0;
}

static void *__get(Gridlayout *obj, char *attrib)
{
    if (strcmp(attrib, "name") == 0) {
        return obj->name;
    } else if(strcmp(attrib, "row_max") == 0) {
        return &obj->row_max;
    } else if(strcmp(attrib, "col_max") == 0) {
        return &obj->col_max;
    } else {
        dbg_str(DBG_WARNNING,"gridlayout get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int get_x_axis_of_current_grid(Gridlayout *obj)
{
    Gridlayout *l = obj;
    int i, x;

    for ( i = 0, x = 0; i < l->cur_col; i++) {
        x += l->col_width[i];
    }

    return x;
}

static int get_y_axis_of_current_grid(Gridlayout *obj)
{
    Gridlayout *l = obj;
    int i, y;

    for ( i = 0, y = 0; i < l->cur_row; i++) {
        y += l->row_height[i];
    }

    return y;
}

static int __add_component(Gridlayout *obj, void *component)
{
    Gridlayout *l        = obj;
    Container *container = (Container *)obj;
    Map *map             = container->map;
    char buffer[8]       = {0};
    Component *c         = (Component *)component;
    Subject *subject     = (Subject *)c;
    position_t position;
    uint8_t rearrange_comonents_flag = 0;

    if (strcmp(c->name,"") == 0) {
        dbg_str(DBG_WARNNING,"component name is NULL, this is vip, add component failed, please check");
        return -1;
    }

    dbg_str(DBG_IMPORTANT, "add component name %s, component addr %p", c->name,c);

    addr_to_buffer(c,(uint8_t *)buffer);

    position.x = get_x_axis_of_current_grid(obj);
    position.y = get_y_axis_of_current_grid(obj);

    dbg_str(DBG_SUC,"position x=%d, y=%d", position.x, position.y);
    container->update_component_position(c, &position);

    map->insert(map, c->name, buffer);
    dbg_str(DBG_DETAIL,"grid_components:%p, cur_row:%d, cur_col:%d, c:%p",
            l->grid_components, l->cur_row, l->cur_col,c);

    *(l->grid_components + l->cur_row * l->col_max + l->cur_col) = c; 

    if (l->row_height[l->cur_row] < subject->height) {
        l->row_height[l->cur_row] = subject->height;
        rearrange_comonents_flag = 1;
    } 

    if (l->col_width[l->cur_col] < subject->width) {
        l->col_width[l->cur_col] = subject->width;
        rearrange_comonents_flag = 1;
    }

    if (rearrange_comonents_flag == 1) {
        dbg_str(DBG_WARNNING,"not support rearrange component in grid now");
        return -1;
    }
    
    l->cur_col++;
    if(l->cur_col > l->col_max - 1) {
        l->cur_col = 0;
        l->cur_row++;
    }

    return 0;
}

static void load_component_resources(Iterator *iter, void *arg) 
{
    Component *component;
    Window *window = (Window *)arg;
    uint8_t *addr;

    addr = (uint8_t *)iter->get_vpointer(iter);

    component = (Component *)buffer_to_addr(addr);
    if(component->load_resources)
        component->load_resources(component, window);
}


static int __load_resources(Gridlayout *component,void *window)
{
    Container *container = (Container *)component;

    dbg_str(DBG_SUC,"%s load resources",component->name);
    container->for_each_component(container, load_component_resources, window);

    return 0;
}

static int __unload_resources(Gridlayout *component,void *window)
{
    //...........
}

static void draw_component(Iterator *iter, void *arg) 
{
    Component *component;
    uint8_t *addr;
    Graph *g = (Graph *)arg;

    dbg_str(DBG_DETAIL,"draw_component");

    addr = (uint8_t *)iter->get_vpointer(iter);
    component = (Component *)buffer_to_addr(addr);

    if(component->draw) component->draw(component, g);
}

static int __draw(Gridlayout *component, void *graph)
{
    Container *container = (Container *)component;
    Graph *g = (Graph *)graph;
    dbg_str(DBG_SUC,"%s draw", ((Obj *)component)->name);

    container->for_each_component(container, draw_component, g);
}

static class_info_entry_t gridlayout_class_info[] = {
    [0 ] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
    [1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
    [2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
    [3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
    [4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
    [5 ] = {ENTRY_TYPE_FUNC_POINTER,"","add_component",__add_component,sizeof(void *)},
    [6 ] = {ENTRY_TYPE_FUNC_POINTER,"","draw",__draw,sizeof(void *)},
    [7 ] = {ENTRY_TYPE_FUNC_POINTER,"","load_resources",__load_resources,sizeof(void *)},
    [8 ] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
    [9 ] = {ENTRY_TYPE_INT32_T,"int","row_max",NULL,sizeof(int)},
    [10] = {ENTRY_TYPE_INT32_T,"int","col_max",NULL,sizeof(int)},
    [11] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Gridlayout",gridlayout_class_info);

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

void *new_label(allocator_t *allocator, int x, int y, int width, int height, char *name)
{
    Subject *subject;
    char *set_str;
    char buf[2048];

    gen_label_setting_str(x, y, width, height, name, (void *)buf);
    subject   = OBJECT_NEW(allocator, Label,buf);

    object_dump(subject, "Label", buf, 2048);
    dbg_str(DBG_DETAIL,"Label dump: %s",buf);

    return subject;
}
#endif

char *gen_gridlayout_setting_str(int x, int y, int width, int height, char *name, void *out)
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
                    \"Gridlayout\":{\
                        \"row_max\":%d,\
                        \"col_max\":%d\
                    }\
                }";

    sprintf(out, set_str, x, y, width, height,1, name, 2,2);

    /*
     *printf("%s",out);
     */

    return out;
}

void *new_gridlayout(allocator_t *allocator, int x, int y, int width, int height, char *name)
{
    char *set_str;
    char buf[2048];
    Container *container;

    gen_gridlayout_setting_str(x, y, width, height, name, buf);
    container = OBJECT_NEW(allocator, Gridlayout,buf);

    object_dump(container, "Gridlayout", buf, 2048);
    dbg_str(DBG_DETAIL,"Gridlayout dump: %s",buf);

    return container;
}

void test_ui_gridlayout()
{
    Window *window;
    Container *window_container, *grid_container;
    Graph *g;
    Subject *subject;
    __Event *event;
    Label *label;
    allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    char buf[2048];

    set_str          = gen_window_setting_str();
    window           = OBJECT_NEW(allocator, Sdl_Window,set_str);
    g                = window->graph;
    event            = window->event;
    window_container = (Container *)window;

    object_dump(window, "Sdl_Window", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

    grid_container   = new_gridlayout(allocator, 0, 0, 600, 600, "grid_container");

    label            = new_label(allocator,0, 0, 80, 20, "label00");
    grid_container->add_component(grid_container, label);

    label            = new_label(allocator,0, 0, 80, 20, "label01");
    grid_container->add_component(grid_container, label);

    label            = new_label(allocator,0, 0, 80, 20, "label10");
    grid_container->add_component(grid_container, label);

    label            = new_label(allocator,0, 0, 80, 20, "label11");
    grid_container->add_component(grid_container, label);

    window_container->add_component(window_container,grid_container);

    dbg_str(DBG_DETAIL,"window container :%p",window_container);

    window->load_resources(window);
    window->update_window(window);

    event->poll_event(event, window);

    object_destroy(window);
}
