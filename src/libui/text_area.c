/**
 * @file text_area.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/text_area.h>
#include <libui/sdl_window.h>
#include <libui/character.h>
#include <libui/timer.h>

extern void print_line_info(Iterator *iter);
extern char *global_text;

char get_row_at_cursor(Component *component) 
{
	Text_Area *ta    = (Text_Area *)component;
    cursor_t *cursor = &ta->cursor;
    uint16_t row;

	row = ta->start_line +  cursor->y / cursor->height;

    return row;
}

void move_cursor_left(Component *component) 
{
	Text_Area *ta          = (Text_Area *)component;
	Window *window         = (Window *)ta->window;
	Graph *g               = ((Window *)window)->graph;
    cursor_t *cursor       = &ta->cursor;
	text_line_t *line_info = NULL;
	Character *character;
    uint16_t cursor_line;
	char c = 0;

	cursor_line    = get_row_at_cursor(component);
	line_info      = (text_line_t *)ta->text->get_text_line_info(ta->text, cursor_line);

	if(cursor->x == 0) {
		dbg_str(DBG_DETAIL,"cursor x=%d width=%d",cursor->x, cursor->width);
		return;
    } 

    c              = line_info->head[cursor->offset - 1];
    character      = (Character *)g->font->ascii[c].character;

    cursor->c      = c;
    cursor->x     -= character->width;
    cursor->width  = character->width;
    cursor->offset--;

    dbg_str(DBG_DETAIL,"offset=%d, char =%c, x pos=%d, char_width =%d",
            cursor->offset, cursor->c,cursor->x, character->width);

    return ;
}

void move_cursor_right(Component *component) 
{
	Text_Area *ta          = (Text_Area *)component;
	Window *window         = (Window *)ta->window;
	Graph *g               = ((Window *)window)->graph;
    cursor_t *cursor       = &ta->cursor;
    int width              = ((Subject *)component)->width;
	text_line_t *line_info = NULL;
	Character *character;
    uint16_t cursor_line;
	char c = 0;

	cursor_line    = get_row_at_cursor(component);
	line_info      = (text_line_t *)ta->text->get_text_line_info(ta->text, cursor_line);

	if(cursor->x + cursor->width < line_info->line_lenth) {
		c              = line_info->head[cursor->offset + 1];
		character      = (Character *)g->font->ascii[c].character;

		if(c == '\n') {
			cursor->c      = ' ';
		} else {
			cursor->c      = c;
		}
		cursor->x     += cursor->width;
		cursor->width  = character->width;
		cursor->offset++;

		dbg_str(DBG_DETAIL,"offset=%d, char =%c, x pos=%d, char_width =%d",
				cursor->offset, cursor->c,cursor->x, character->width);
	/*
	 *} else if(cursor->x + cursor->width == line_info->line_lenth && 
	 *        width - line_info->line_lenth >= ta->char_min_width) {
	 *    cursor->c      = ' ';
	 */
	} else {
		return;
	}


    return ;
}

void move_cursor_up(Component *component) 
{
	Text_Area *ta          = (Text_Area *)component;
	Window *window         = (Window *)ta->window;
	Graph *g               = ((Window *)window)->graph;
    cursor_t *cursor       = &ta->cursor;
    int component_width    = ((Subject *)component)->width;
	int head_offset, i     = 0, width_sum = 0;
	text_line_t *line_info = NULL;
	Character *character;
    uint16_t cursor_line;
	char c = 0;

	cursor_line = get_row_at_cursor(component);
	if(ta->start_line == cursor_line && cursor_line == 0) {
		return;
	}

	line_info = (text_line_t *)ta->text->get_text_line_info(ta->text, cursor_line - 1);
	/*
	 *dbg_str(DBG_DETAIL,"cursor line=%d, data:%s", cursor_line, line_info->string->value);
	 */

	if(cursor->y >= cursor->height) {
		cursor->y -= cursor->height;

		if(line_info->line_lenth > cursor->x) {

			for( i = 0; width_sum < line_info->line_lenth; i++) {
				c         = line_info->head[i];
				character = (Character *)g->font->ascii[c].character;

				if(cursor->x >= width_sum && cursor->x <  character->width  + width_sum) {
					cursor->x      = width_sum;
					if(c == '\n') {
						cursor->c      = ' ';
					} else {
						cursor->c      = c;
					}
					cursor->offset = i;
					cursor->width  = character->width;
					cursor->height = character->height;

                    dbg_str(DBG_DETAIL,"offset=%d, char =%c, x pos=%d, char_width =%d",
                            cursor->offset, cursor->c,cursor->x, character->width);
                    break;
				} else {
					width_sum += character->width;
				}
			}
		} else {
			if((c = *line_info->tail) == '\n') {
				character = (Character *)g->font->ascii[c].character;
				cursor->x      = line_info->line_lenth - character->width;
				cursor->c      = ' ';
				cursor->offset = line_info->tail - line_info->head;
				cursor->width  = character->width;
				cursor->height = character->height;
			} else {
				character = (Character *)g->font->ascii[c].character;
				cursor->x      = line_info->line_lenth - character->width;
				cursor->c      = c;
				cursor->offset = line_info->tail - line_info->head - 1;
				cursor->width  = character->width;
				cursor->height = character->height;
			}

		}
	} else {
		//line down
		cursor_t bak  = *cursor;
		bak.y        += bak.height;
		ta->one_line_down(component, g);
		*cursor = bak;
		move_cursor_up(component);
		return ;
	}

	return ;
}

void move_cursor_down(Component *component) 
{
	Text_Area *ta          = (Text_Area *)component;
	Window *window         = (Window *)ta->window;
	Graph *g               = ((Window *)window)->graph;
	cursor_t *cursor       = &ta->cursor;
    int component_height   = ((Subject *)component)->height;
    int component_width    = ((Subject *)component)->width;
	int head_offset, i     = 0, width_sum = 0;
	char c                 = 0;
	text_line_t *line_info = NULL;
	Character *character;
    uint16_t cursor_line,  total_line_num;

	cursor_line    = get_row_at_cursor(component);
	total_line_num = ta->text->get_line_count(ta->text);

	if(cursor_line == total_line_num) {
		dbg_str(DBG_DETAIL, "move_cursor_down, already last line, "
				"cursor_line =%d, total_line_num =%d", cursor_line, total_line_num);
		return;
	}

	line_info = (text_line_t *)ta->text->get_text_line_info(ta->text, cursor_line + 1);

	if(cursor->y + cursor->height * 2 < component_height) { /*not last line*/
		if(line_info->line_lenth > cursor->x) { /*next line line_lenth greater than cursor pos*/
			cursor->y   += cursor->height;
			for( i = 0; ; i++) {
				c         = line_info->head[i];
				character = (Character *)g->font->ascii[c].character;
				if(cursor->x >= width_sum && cursor->x < width_sum + character->width) {
					cursor->x      = width_sum;
					cursor->c      = c;
					if(c == '\n') {
						cursor->c      = ' ';
					} else {
						cursor->c      = c;
					}
					cursor->offset = i;
					cursor->width  = character->width;
					cursor->height = character->height;

                    dbg_str(DBG_DETAIL,"offset=%d, char =%c, x pos=%d, char_width =%d",
                            cursor->offset, cursor->c,cursor->x, character->width);
					break;
				} else {
					width_sum += character->width;
				}
			}
		}  else {
			cursor->y += cursor->height;
			if((c = *line_info->tail) == '\n') {
				character = (Character *)g->font->ascii[c].character;
				cursor->x      = line_info->line_lenth - character->width;
				cursor->c      = ' ';
				cursor->offset = line_info->tail - line_info->head;
				cursor->width  = character->width;
				cursor->height = character->height;
			} else {
				character = (Character *)g->font->ascii[c].character;
				cursor->x      = line_info->line_lenth - character->width;
				cursor->c      = c;
				cursor->offset = line_info->tail - line_info->head - 1;
				cursor->width  = character->width;
				cursor->height = character->height;
			}

		}
	}  else { /*last line*/
		cursor_t bak  = *cursor;
		bak.y        -= bak.height;
		ta->one_line_up(component, g);
		*cursor       = bak;
		move_cursor_down(component);
		//line up
	}


	return ;
}

static int draw_cursor(Component *component,void *graph)
{
	Text_Area *ta     = (Text_Area *)component;
	Window *window    = (Window *)ta->window;
	Graph *g          = ((Window *)window)->graph;
    cursor_t *cursor  = &ta->cursor;
    color_t *ft_color = &ta->front_color;
    color_t *bg_color = &ta->background_color;
    static int count  = 0;
	Character *character;   
    char c;

	c         = cursor->c;

    character = g->render_load_character(g,c,g->font,
                                         bg_color->r,
                                         bg_color->g, 
                                         bg_color->b, 
                                         bg_color->a); 
	/*
	 *dbg_str(DBG_DETAIL,"draw character c :%c",c);
	 */
    g->render_set_color(g,ft_color->r,ft_color->g,ft_color->b,ft_color->a);
    g->render_fill_rect(g,cursor->x, cursor->y, character->width, character->height);
    g->render_write_character(g,cursor->x,cursor->y,character);
	g->render_present(g);

    object_destroy(character);   
}

static int reverse_cursor(Component *component,void *graph)
{
	Text_Area *ta     = (Text_Area *)component;
	Window *window    = (Window *)ta->window;
	Graph *g          = ((Window *)window)->graph;
    cursor_t *cursor  = &ta->cursor;
    color_t *ft_color = &ta->front_color;
    color_t *bg_color = &ta->background_color;
	Character *character;   
    char c;

	c         = cursor->c;

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

static int draw_character(Component *component,char c, void *graph)
{
    Text_Area *ta    = (Text_Area *)component;
	Graph *g         = (Graph *)graph;
    cursor_t *cursor = &ta->cursor;
	Character *character;

	character = (Character *)g->font->ascii[c].character;
    if(character->height == 0) {
        dbg_str(DBG_WARNNING,"text list may have problem");
    }
    if(cursor->x + character->width > ((Subject *)component)->width) {
        cursor->x       = 0;
		cursor->y      += character->height;
        cursor->height  = character->height;
		cursor->width   = character->width;
		cursor->offset  = 0;
    }

	if(character->code == '\n') {
		cursor->x       = 0;
		cursor->y      += character->height;
		cursor->width   = 0;
        cursor->height  = character->height;
		cursor->offset  = 0;
	} else {
		cursor->x      += character->width;
		g->render_write_character(g,cursor->x - character->width,cursor->y,character);/**/
		cursor->width   = character->width;
        cursor->height  = character->height;
		cursor->offset++;
	}

	cursor->c = ' ';

	return 0;
}

static int erase_character(Component *component,char c, void *graph)
{
	Text_Area *ta     = (Text_Area *)component;
	Window *window    = (Window *)ta->window;
	Graph *g          = ((Window *)window)->graph;
    cursor_t *cursor  = &ta->cursor;
    color_t *ft_color = &ta->front_color;
    color_t *bg_color = &ta->background_color;
	Character *character;   

    character = g->render_load_character(g,c,g->font,
                                         ft_color->r,
                                         ft_color->g,
                                         ft_color->b,
                                         ft_color->a); 

    g->render_set_color(g,bg_color->r, bg_color->g, bg_color->b, bg_color->a);
    g->render_fill_rect(g,cursor->x, cursor->y, character->width, character->height);

	g->render_present(g);

    object_destroy(character);   
}

static uint32_t cursor_timer_callback(uint32_t interval, void* param )
{
	__Timer *timer = (__Timer *)param;
	Text_Area *ta  = (Text_Area *)timer->opaque;
	Window *window = (Window *)ta->window;
	Graph *g       = ((Window *)window)->graph;

	/*
	 *dbg_str(DBG_DETAIL,"cursor_timer_callback");
	 */

    if((ta->cursor_count++ % 2) == 0) {
        reverse_cursor((Component *)ta,g);
    } else {
        draw_cursor((Component *)ta,g);
    }

	window->remove_timer(window, timer);
	timer->reuse(timer);
}

static int __construct(Text_Area *ta,char *init_str)
{
    allocator_t *allocator = ((Obj *)ta)->allocator;
    cursor_t *cursor       = &ta->cursor;

	dbg_str(DBG_SUC,"ta construct");

    ta->string             = OBJECT_NEW(allocator, String, NULL);
    ta->string->assign(ta->string,global_text);

    ta->text               = OBJECT_NEW(allocator, Text,"");
	ta->text->content      = ta->string->value;
	ta->start_line         = 0;

    ta->front_color.r      = 0;
    ta->front_color.g      = 0;
    ta->front_color.b      = 0;
    ta->front_color.a      = 0xff;

    ta->background_color.r = 0xff;
    ta->background_color.g = 0xff;
    ta->background_color.b = 0xff;
    ta->background_color.a = 0xff;

    ta->cursor_count       = 0;
    cursor->x              = 0;
    cursor->y              = 0;

	return 0;
}

static int __deconstrcut(Text_Area *ta)
{
	dbg_str(DBG_SUC,"ta deconstruct");

    object_destroy(ta->string);
    object_destroy(ta->text);
	if(ta->timer) {
		object_destroy(ta->timer);
	}

	return 0;
}

static int __set(Text_Area *ta, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		ta->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		ta->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		ta->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		ta->deconstruct = value;
	}
	/*vitual methods*/
	else if(strcmp(attrib, "draw") == 0) {
		ta->draw = value;
	} else if(strcmp(attrib, "load_resources") == 0) {
		ta->load_resources = value;
	} else if(strcmp(attrib, "text_key_input") == 0) {
		ta->text_key_input = value;
	} else if(strcmp(attrib, "backspace_key_input") == 0) {
		ta->backspace_key_input = value;
	} else if(strcmp(attrib, "up_key_down") == 0) {
		ta->up_key_down = value;
	} else if(strcmp(attrib, "down_key_down") == 0) {
		ta->down_key_down = value;
	} else if(strcmp(attrib, "left_key_down") == 0) {
		ta->left_key_down = value;
	} else if(strcmp(attrib, "right_key_down") == 0) {
		ta->right_key_down = value;
	} else if(strcmp(attrib, "pageup_key_down") == 0) {
		ta->pageup_key_down = value;
	} else if(strcmp(attrib, "pagedown_key_down") == 0) {
		ta->pagedown_key_down = value;
	} else if(strcmp(attrib, "one_line_up") == 0) {
		ta->one_line_up = value;
	} else if(strcmp(attrib, "one_line_down") == 0) {
		ta->one_line_down = value;
	}
	/*attribs*/
	else if(strcmp(attrib, "name") == 0) {
        strncpy(ta->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"ta set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Text_Area *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"text_area get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __load_resources(Component *component,void *window)
{
	Graph *g          = ((Window *)window)->graph;
	Text_Area *ta     = (Text_Area *)component;
	Character *character;

	dbg_str(DBG_SUC,"%s load load_resources",component->name);

    ta->window        = window;

	g->font->load_ascii_character(g->font,g);
	ta->text->write_text(ta->text,0, ta->text->content, g->font);
	/*
	 *ta->text->line_info->for_each(ta->text->line_info, print_line_info);
	 */

	ta->timer         = ((Window *)window)->create_timer(window);
	ta->timer->opaque = component;
	ta->timer->set_timer(ta->timer, 1 * 500, cursor_timer_callback); 

	character         = (Character *)g->font->ascii['a'].character;
	ta->cursor.height = character->height;

	character          = (Character *)g->font->ascii['i'].character;
	ta->char_min_width = character->width;

	dbg_str(DBG_DETAIL,"cursor height =%d",ta->cursor.height);

	return 0;
}

static int __unload_resources(Component *component,void *window)
{
    //...........
}

static int __draw(Component *component, void *graph)
{
	Text_Area *ta    = (Text_Area *)component;
	Graph *g         = (Graph *)graph;
	Subject *s       = (Subject *)component;
    cursor_t *cursor = &ta->cursor;
	text_line_t *line_info = NULL;
    int i, j;
    char c;

    /*
	 *dbg_str(DBG_DETAIL,"%s draw", ((Obj *)component)->name);
     */
	g->render_set_color(g,0xff,0xff,0xff,0xff);
	g->render_clear(g);

	g->render_draw_rect(g,s->x,s->y,s->width,s->height);

	cursor->x = 0; cursor->y = 0; cursor->width = 0; 

	for(j = ta->start_line; cursor->y < ((Subject *)component)->height; j++) {
		line_info = (text_line_t *)ta->text->get_text_line_info(ta->text,j);
        if(line_info == NULL) break;

		for(i = 0; i < line_info->tail - line_info->head + 1; i++) {
			c = line_info->head[i];
			draw_character(component,c, graph);
		}
	}

	g->render_present(g);
}

static int __text_key_input(Component *component,char c, void *graph)
{
#define MAX_MODULATE_STR_LEN 1024
	Graph *g                       = (Graph *)graph;
    Text_Area *ta                  = (Text_Area *)component;
    Text *text                     = ta->text;
    cursor_t *cursor               = &ta->cursor;
    color_t *ft_color              = &ta->front_color;
    color_t *bg_color              = &ta->background_color;
    int disturbed_line_count       = 0;
    int disturbed_str_len          = 0;
    char str[MAX_MODULATE_STR_LEN] = {0};
	Character *character;
    uint16_t cursor_line;


#if 0
    disturbed_line_count = extract_line_text_disturbed_by_inserting(component,
																	str,
																	MAX_MODULATE_STR_LEN);
    if(disturbed_line_count < 0) {
        return -1;
    }
	disturbed_str_len = strlen(str);
	
    dbg_str(DBG_DETAIL,"text_line_disturbed_by_inserting:%s",str);

	if(disturbed_str_len == 0) {
		dbg_str(DBG_DETAIL,"append_char");
	} else {
		dbg_str(DBG_DETAIL,"insert_char");
	}

#endif
	cursor_line          = get_row_at_cursor(component);
    disturbed_line_count = text->write_char(text,cursor_line , cursor->offset, c, g->font);

    /*
     *erase_character(component,c, graph);
     *draw_character(component,c, graph);
	 *g->render_present(g);
     */

    return 0;
#undef MAX_MODULATE_STR_LEN
}

static int __backspace_key_input(Component *component,void *graph)
{
    Text_Area *ta = (Text_Area *)component;
	dbg_str(DBG_DETAIL,"backspace_key_input");
}

static int __up_key_down(Component *component,void *graph)
{
	Graph *g          = (Graph *)graph;
	Text_Area *ta     = (Text_Area *)component;
    cursor_t *cursor  = &ta->cursor;
    color_t *bg_color = &ta->background_color;

    reverse_cursor(component,g);
	move_cursor_up(component);
    draw_cursor(component,g);

    ta->cursor_count  = 0;

    return 0;
}

static int __down_key_down(Component *component,void *graph)
{
	Graph *g         = (Graph *)graph;
	Text_Area *ta    = (Text_Area *)component;
    cursor_t *cursor = &ta->cursor;

    reverse_cursor(component,g);
	move_cursor_down(component);
    draw_cursor(component,g);

    ta->cursor_count = 0;

    return 0;
}

static int __left_key_down(Component *component,void *graph)
{
	Graph *g         = (Graph *)graph;
	Text_Area *ta    = (Text_Area *)component;
    cursor_t *cursor = &ta->cursor;

	/*
	 *dbg_str(DBG_DETAIL,"left_key_down");
	 */

    reverse_cursor(component,g);
	move_cursor_left(component);
    draw_cursor(component,g);

    ta->cursor_count = 0;
}

static int __right_key_down(Component *component,void *graph)
{
	Graph *g         = (Graph *)graph;
	Text_Area *ta    = (Text_Area *)component;
    cursor_t *cursor = &ta->cursor;

	/*
	 *dbg_str(DBG_DETAIL,"right_key_down");
	 */

    reverse_cursor(component,g);
	move_cursor_right(component);
    draw_cursor(component,g);

    ta->cursor_count = 0;
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
	Graph *g         = (Graph *)graph;
    Text_Area *ta    = (Text_Area *)component;
    cursor_t *cursor = &ta->cursor;

	dbg_str(DBG_DETAIL,"one line up");

	cursor->y        = 0;
	cursor->x        = 0;
	ta->start_line++;
	ta->draw(component,graph); 
}

static int __one_line_down(Component *component,void *graph)
{
    Text_Area *ta    = (Text_Area *)component;
	Graph *g         = (Graph *)graph;
    cursor_t *cursor = &ta->cursor;

	dbg_str(DBG_DETAIL,"one line down");

	cursor->y        = 0;
	cursor->x        = 0;

	if(ta->start_line) {
		ta->start_line--;
		ta->draw(component,graph); 
	} else if(ta->start_line == 0) {
		ta->draw(component,graph); 
	}

}

static class_info_entry_t text_area_class_info[] = {
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
REGISTER_CLASS("Text_Area",text_area_class_info);

char *gen_text_area_setting_str()
{
    cjson_t *root,*b, *c, *e, *s;
    char *set_str;

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Text_Area", b = cjson_create_object());{
            cjson_add_item_to_object(root, "Component", c = cjson_create_object());{
                cjson_add_item_to_object(root, "Container", e = cjson_create_object());{
                    cjson_add_item_to_object(e, "Subject", s = cjson_create_object());{
                        cjson_add_number_to_object(s, "x", 0);
                        cjson_add_number_to_object(s, "y", 0);
                        cjson_add_number_to_object(s, "width", 600);
                        cjson_add_number_to_object(s, "height", 600);
                    }
                }
				cjson_add_string_to_object(c, "name", "text_area");
            }
        }
    }
    set_str = cjson_print(root);

    return set_str;
}
void test_ui_text_area()
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

    set_str = gen_text_area_setting_str();
    subject = OBJECT_NEW(allocator, Text_Area,set_str);

    object_dump(subject, "Text_Area", buf, 2048);
    dbg_str(DBG_DETAIL,"Text_Area dump: %s",buf);

    container->add_component(container,subject);
	/*
     *container->search_component(container,"text_area");
	 */
	dbg_str(DBG_DETAIL,"window container :%p",container);

	window->load_resources(window);
	window->update_window(window);

    event->poll_event(event, window);

    object_destroy(window);

    free(set_str);

}


