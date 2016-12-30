/**
 * @file box.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/box.h>
#include <libui/sdl_window.h>
#include <libui/character.h>
#include <libui/timer.h>

extern void print_line_info(Iterator *iter);
extern char *global_text;

static int __construct(Box *box,char *init_str)
{
    allocator_t *allocator = ((Obj *)box)->allocator;
    cursor_t *cursor = &box->cursor;

	dbg_str(DBG_SUC,"box construct");

    box->string = OBJECT_NEW(allocator, String, NULL);
    box->string->assign(box->string,global_text);

    box->text = OBJECT_NEW(allocator, Text,"");
	box->text->content = box->string->value;
	box->start_line = 0;

    box->front_color.r = 0;
    box->front_color.g = 0;
    box->front_color.b = 0;
    box->front_color.a = 0xff;

    box->background_color.r = 0xff;
    box->background_color.g = 0xff;
    box->background_color.b = 0xff;
    box->background_color.a = 0xff;

    box->cursor_count = 0;
    cursor->x = 0;
    cursor->y = 0;

	return 0;
}

static int __deconstrcut(Box *box)
{
	dbg_str(DBG_SUC,"box deconstruct");
    object_destroy(box->string);
    object_destroy(box->text);
	if(box->timer) {
		object_destroy(box->timer);
	}

	return 0;
}

static int __set(Box *box, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		box->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		box->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		box->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		box->deconstruct = value;
	}
	/*vitual methods*/
	else if(strcmp(attrib, "draw") == 0) {
		box->draw = value;
	} else if(strcmp(attrib, "load_resources") == 0) {
		box->load_resources = value;
	} else if(strcmp(attrib, "text_key_input") == 0) {
		box->text_key_input = value;
	} else if(strcmp(attrib, "backspace_key_input") == 0) {
		box->backspace_key_input = value;
	} else if(strcmp(attrib, "up_key_down") == 0) {
		box->up_key_down = value;
	} else if(strcmp(attrib, "down_key_down") == 0) {
		box->down_key_down = value;
	} else if(strcmp(attrib, "left_key_down") == 0) {
		box->left_key_down = value;
	} else if(strcmp(attrib, "right_key_down") == 0) {
		box->right_key_down = value;
	} else if(strcmp(attrib, "pageup_key_down") == 0) {
		box->pageup_key_down = value;
	} else if(strcmp(attrib, "pagedown_key_down") == 0) {
		box->pagedown_key_down = value;
	} else if(strcmp(attrib, "one_line_up") == 0) {
		box->one_line_up = value;
	} else if(strcmp(attrib, "one_line_down") == 0) {
		box->one_line_down = value;
	}
	/*attribs*/
	else if(strcmp(attrib, "name") == 0) {
        strncpy(box->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"box set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Box *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"box get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

char get_character_at_cursor(Component *component) 
{
	Box *b   = (Box *)component;
    uint16_t line_num;

    return 'c';
}


static int draw_cursor(Component *component,void *graph)
{
	Box *b   = (Box *)component;
	Window *window = (Window *)b->window;
	Graph *g = ((Window *)window)->graph;
    cursor_t *cursor = &b->cursor;
	Character *character;   
    color_t *ft_color = &b->front_color;
    color_t *bg_color = &b->background_color;
    char c;
    static count = 0;

    c = get_character_at_cursor(component);

    character = g->render_load_character(g,c,g->font,
                                         bg_color->r,
                                         bg_color->g, 
                                         bg_color->b, 
                                         bg_color->a); 

    g->render_set_color(g,ft_color->r,ft_color->g,ft_color->b,ft_color->a);
    g->render_fill_rect(g,cursor->x, cursor->y, character->width, character->height);
    g->render_write_character(g,cursor->x,cursor->y,character);
	g->render_present(g);

    object_destroy(character);   
}

static int erase_cursor(Component *component,void *graph)
{
	Box *b   = (Box *)component;
	Window *window = (Window *)b->window;
	Graph *g = ((Window *)window)->graph;
    cursor_t *cursor = &b->cursor;
	Character *character;   
    color_t *ft_color = &b->front_color;
    color_t *bg_color = &b->background_color;
    char c;

    c = get_character_at_cursor(component);

    character = g->render_load_character(g,c,g->font,
                                         ft_color->r,
                                         ft_color->g,
                                         ft_color->b,
                                         ft_color->a); 

    g->render_set_color(g,bg_color->r, bg_color->g, bg_color->b, bg_color->a);
    g->render_fill_rect(g,cursor->x, cursor->y, character->width, character->height);

    g->render_write_character(g,cursor->x,cursor->y,character);
	g->render_present(g);

    object_destroy(character);   
}

static uint32_t cursor_timer_callback(uint32_t interval, void* param )
{
	__Timer *timer = (__Timer *)param;
	Box *box   = (Box *)timer->opaque;
	Window *window = (Window *)box->window;
	Graph *g = ((Window *)window)->graph;

	dbg_str(DBG_DETAIL,"cursor_timer_callback");

    if((box->cursor_count++ % 2) == 0) {
        erase_cursor((Component *)box,g);
    } else {
        draw_cursor((Component *)box,g);
    }

	window->remove_timer(window, timer);
	timer->reuse(timer);
}

static int __load_resources(Component *component,void *window)
{
	Graph *g = ((Window *)window)->graph;
	Box *b   = (Box *)component;

	dbg_str(DBG_SUC,"%s load load_resources",component->name);

    b->window = window;

	g->font->load_ascii_character(g->font,g);
	b->text->parse_text(b->text, 0, g->font);
	b->text->line_info->for_each(b->text->line_info, print_line_info);

	b->timer         = ((Window *)window)->create_timer(window);
	b->timer->opaque = component;
	b->timer->set_timer(b->timer, 1 * 500, cursor_timer_callback);
}

static int __unload_resources(Component *component,void *window)
{
    //...........
}

#if 1
static int write_character(Component *component,char c, void *graph)
{
    Box *b   = (Box *)component;
	Graph *g = (Graph *)graph;
    cursor_t *cursor = &b->cursor;
	Character *character;

	character = (Character *)g->font->ascii[c].character;
    if(cursor->x + character->width > ((Subject *)component)->width) {
        cursor->x       = 0;
        cursor->height  = 0;
        cursor->y      += character->height;
    }
    if(cursor->height < cursor->y) {
        cursor->height = cursor->y;
    }
	if(character->code == '\n') {
		cursor->x       = 0;
		cursor->y      += character->height;
		cursor->height  = 0;
	} else {
		g->render_write_character(g,cursor->x,cursor->y,character);
		cursor->x += character->width;
	}
}
#else
static int write_character(Component *component,char c, void *graph)
{
	Graph *g = (Graph *)graph;
	Character *character;
    Box *b = (Box *)component;

	character = (Character *)g->render_load_character(g,(uint32_t)c,g->font, 0,0,0,0xff);
    if(cursor->x + character->width > ((Subject *)component)->width) {
        cursor->x = 0;
        cursor->height = 0;
        cursor->y += character->height;
    }
    if(cursor->height < cursor->y) {
        cursor->height = cursor->y;
    }
	g->render_write_character(g,cursor->x,cursor->y,character);
	cursor->x += character->width;

    object_destroy(character);

}
#endif

static int __draw(Component *component, void *graph)
{
	Box *b     = (Box *)component;
	Graph *g   = (Graph *)graph;
	Subject *s = (Subject *)component;
    cursor_t *cursor = &b->cursor;
    int i, start;
    char c;

	dbg_str(DBG_DETAIL,"%s draw", ((Obj *)component)->name);
	g->render_set_color(g,0xff,0xff,0xff,0xff);
	g->render_clear(g);

	start = b->text->get_head_offset_of_line(b->text, b->start_line);
	dbg_str(DBG_DETAIL,"start line =%d start offset =%d",b->start_line, start);
	if(start < 0) {
		dbg_str(DBG_WARNNING,"get head offset of one line err");
		return -1;
	}

	g->render_draw_rect(g,s->x,s->y,s->width,s->height);

	dbg_str(DBG_DETAIL,"draw x=%d, y=%d", cursor->x, cursor->y);
	char buf[10] = {0};
	memcpy(buf, b->string->value + start, 9);
	dbg_str(DBG_DETAIL,"%s", buf);

    for(i = 0; i < strlen(b->string->value + start); i++) {
        c = b->string->value[i + start];
        write_character(component,c, graph);
        if(cursor->y > ((Subject *)component)->height ){
            dbg_str(DBG_DETAIL,"box y =%d , subject height =%d",
					cursor->y,
					((Subject *)component)->height);
            break;
        }
    }

	g->render_present(g);
}

static int __text_key_input(Component *component,char c, void *graph)
{
	Graph *g = (Graph *)graph;
	Character *character;
    Box *b = (Box *)component;
    cursor_t *cursor = &b->cursor;

	dbg_str(DBG_DETAIL,"text input");

	character = (Character *)g->render_load_character(g,(uint32_t)c,g->font, 0,0,0,0xff);
    if(cursor->x + character->width > ((Subject *)component)->width) {
        cursor->x       = 0;
        cursor->height  = 0;
        cursor->y      += character->height;
    }
    if(cursor->height < cursor->y) {
        cursor->height = cursor->y;
    }
	g->render_write_character(g,cursor->x,cursor->height,character);
	cursor->x += character->width;
	g->render_present(g);

    object_destroy(character);

}

static int __backspace_key_input(Component *component,void *graph)
{
    Box *b = (Box *)component;
	dbg_str(DBG_DETAIL,"backspace_key_input");
}

static int __up_key_down(Component *component,void *graph)
{
	Character *character;
	Graph *g   = (Graph *)graph;
	Box *b     = (Box *)component;
    cursor_t *cursor = &b->cursor;
    color_t *bg_color = &b->background_color;
    char c;

    c = get_character_at_cursor(component);

	character = (Character *)g->font->ascii[c].character;

    erase_cursor(component,g);

    if(cursor->y >= character->height)
        cursor->y -= character->height;

    draw_cursor(component,g);

    b->cursor_count = 0;

    return 0;
}

static int __down_key_down(Component *component,void *graph)
{
	Character *character;
	Graph *g   = (Graph *)graph;
	Box *b     = (Box *)component;
    cursor_t *cursor = &b->cursor;
    color_t *bg_color = &b->background_color;
    int height = ((Subject *)component)->height;
    char c;

    c = get_character_at_cursor(component);

	character = (Character *)g->font->ascii[c].character;

    erase_cursor(component,g);

    if(cursor->y + character->height < height)
        cursor->y += character->height;

    draw_cursor(component,g);

    b->cursor_count = 0;

    return 0;
}

static int __left_key_down(Component *component,void *graph)
{
	dbg_str(DBG_DETAIL,"left_key_down");
}

static int __right_key_down(Component *component,void *graph)
{
	dbg_str(DBG_DETAIL,"right_key_down");
}

static int __pgup_key_down(Component *component,void *graph)
{
	dbg_str(DBG_DETAIL,"pgup_key_down");
}

static int __pgdown_key_down(Component *component,void *graph)
{
	dbg_str(DBG_DETAIL,"pgdown_key_down");
}

static int __one_line_up(Component *component,void *graph)
{
	Graph *g = (Graph *)graph;
    Box *b = (Box *)component;
    cursor_t *cursor = &b->cursor;
	dbg_str(DBG_DETAIL,"up_key_down");

	cursor->y = 0;
	cursor->x = 0;
	b->start_line++;
	/*
	 *if(b->start_line > b->text->total_line_num) {
	 *    b->start_line = b->text->total_line_num;
	 *}
	 */
	b->draw(component,graph); 
	dbg_str(DBG_DETAIL,"start line=%d",b->start_line);
}

static int __one_line_down(Component *component,void *graph)
{
    Box *b = (Box *)component;
	Graph *g = (Graph *)graph;
    cursor_t *cursor = &b->cursor;

	dbg_str(DBG_DETAIL,"down_key_down");

	cursor->y = 0;
	cursor->x = 0;

	if(b->start_line) {
		b->start_line--;
		b->draw(component,graph); 
	} else if(b->start_line == 0) {
		b->draw(component,graph); 
	}

	dbg_str(DBG_DETAIL,"start line=%d",b->start_line);
}

static class_info_entry_t box_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","draw",__draw,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_FUNC_POINTER,"","load_resources",__load_resources,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_FUNC_POINTER,"","text_key_input",__text_key_input,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_FUNC_POINTER,"","backspace_key_input",__backspace_key_input,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_FUNC_POINTER,"","up_key_down",__up_key_down,sizeof(void *)},
	[10] = {ENTRY_TYPE_FUNC_POINTER,"","down_key_down",__down_key_down,sizeof(void *)},
	[11] = {ENTRY_TYPE_FUNC_POINTER,"","left_key_down",__left_key_down,sizeof(void *)},
	[12] = {ENTRY_TYPE_FUNC_POINTER,"","right_key_down",__right_key_down,sizeof(void *)},
	[13] = {ENTRY_TYPE_FUNC_POINTER,"","pageup_key_down",__pgup_key_down,sizeof(void *)},
	[14] = {ENTRY_TYPE_FUNC_POINTER,"","pagedown_key_down",__pgdown_key_down,sizeof(void *)},
	[15] = {ENTRY_TYPE_FUNC_POINTER,"","one_line_up",__one_line_up,sizeof(void *)},
	[16] = {ENTRY_TYPE_FUNC_POINTER,"","one_line_down",__one_line_down,sizeof(void *)},
	[17] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[18] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Box",box_class_info);

char *gen_box_setting_str()
{
    cjson_t *root,*b, *c, *e, *s;
    char *set_str;

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Box", b = cjson_create_object());{
            cjson_add_item_to_object(root, "Component", c = cjson_create_object());{
                cjson_add_item_to_object(root, "Container", e = cjson_create_object());{
                    cjson_add_item_to_object(e, "Subject", s = cjson_create_object());{
                        cjson_add_number_to_object(s, "x", 0);
                        cjson_add_number_to_object(s, "y", 0);
                        cjson_add_number_to_object(s, "width", 600);
                        cjson_add_number_to_object(s, "height", 600);
                    }
                }
				cjson_add_string_to_object(c, "name", "box");
            }
        }
    }
    set_str = cjson_print(root);

    return set_str;
}
void test_ui_box()
{
    Window *window;
    Container *container;
	Graph *g;
    Subject *subject;
	Event *event;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    char buf[2048];

    set_str   = gen_window_setting_str();
    window    = OBJECT_NEW(allocator, Sdl_Window,set_str);
	g         = window->graph;
    event     = window->event;
    container = (Container *)window;

    object_dump(window, "Sdl_Window", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

    set_str = gen_box_setting_str();
    subject = OBJECT_NEW(allocator, Box,set_str);

    object_dump(subject, "Box", buf, 2048);
    dbg_str(DBG_DETAIL,"Box dump: %s",buf);

    container->add_component(container,subject);
	/*
     *container->search_component(container,"box");
	 */
	dbg_str(DBG_DETAIL,"window container :%p",container);

	window->load_resources(window);
	window->update_window(window);

    event->poll_event(event, window);

    object_destroy(window);

    free(set_str);

}


