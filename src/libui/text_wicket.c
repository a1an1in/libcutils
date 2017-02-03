/**
 * @file text_wicket.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/text_wicket.h>
#include <libui/sdl_window.h>
#include <libui/character.h>
#include <libui/timer.h>

extern void print_line_info(Iterator *iter);
extern char *global_text;

char get_row_at_cursor(Component *component) 
{
	Text_Wicket *tw  = (Text_Wicket *)component;
    cursor_t *cursor = &tw->cursor;
    uint16_t row;

	row = tw->start_line + cursor->y / cursor->height;

    return row;
}

int move_cursor_left(Component *component) 
{
	Text_Wicket *tw        = (Text_Wicket *)component;
    Text *text             = tw->text;
	Window *window         = (Window *)tw->window;
	Graph *g               = ((Window *)window)->graph;
    cursor_t *cursor       = &tw->cursor;
	text_line_t *line_info = NULL;
	char c                 = 0;
	Character *character;
    uint16_t cursor_line;

    cursor_line = get_row_at_cursor(component);
    line_info   = (text_line_t *)text->get_text_line_info(text, cursor_line);

    if (cursor->x > 0) {
        c                 = line_info->head[cursor->offset - 1];
        character         = (Character *)g->font->ascii[c].character;
        cursor->c         = c;
        cursor->x        -= character->width;
        cursor->width     = character->width;
        cursor->offset--;
    } else if (cursor->x == 0 && cursor->y > 0) {
		line_info         = (text_line_t *)text->get_text_line_info(text, cursor_line - 1);
        c                 = *line_info->tail;
        character         = (Character *)g->font->ascii[c].character;
        cursor->c         = c;
        cursor->x         = line_info->line_lenth - character->width;
        cursor->y        -= character->height;
        cursor->width     = character->width;
        cursor->offset    = line_info->tail - line_info->head;
    } else if (cursor->x == 0 && cursor->y == 0 && tw->start_line > 0) {
		dbg_str(DBG_DETAIL,"at head of screan");
		return 0;
    } else if (cursor->x == 0 && cursor->y == 0 && tw->start_line == 0) {
		dbg_str(DBG_DETAIL,"already at head of text");
        return 0;
    }

	dbg_str(DBG_DETAIL,
			"offset=%d,cursor_line=%d,last_line_num=%d,char =%c,"
			"x pos=%d,char_width =%d",
			cursor->offset,cursor_line, text->last_line_num, 
			cursor->c, cursor->x, character->width);

    return 1;
}

void move_cursor_right(Component *component) 
{
	Text_Wicket *tw        = (Text_Wicket *)component;
    Text *text             = tw->text;
	Window *window         = (Window *)tw->window;
	Graph *g               = ((Window *)window)->graph;
    cursor_t *cursor       = &tw->cursor;
    int width              = ((Subject *)component)->width;
	text_line_t *line_info = NULL;
	Character *character;
    uint16_t cursor_line;
	char c = 0;

	cursor_line = get_row_at_cursor(component);
	line_info   = (text_line_t *)text->get_text_line_info(text, cursor_line);

	if (cursor->x + cursor->width < line_info->line_lenth) {
		c              = line_info->head[cursor->offset + 1];
		character      = (Character *)g->font->ascii[c].character;

        cursor->c      = c;
		cursor->x     += cursor->width;
		cursor->width  = character->width;
		cursor->offset++;
	} 
	/*when insert char at end of line, may be the case:
	 * 1.cursor->x == line_info->line_lenth
	 * 2.cursor->x + cursor->width > line_info->line_lenth;
	 *if just move cursor in text, only has one case:
	 * 1.cursor->x + cursor->width == line_info->line_lenth
	 * */
	else if (cursor->x + cursor->width > line_info->line_lenth &&
             cursor_line < text->last_line_num)
    {
		line_info       = (text_line_t *)text->get_text_line_info(text, cursor_line + 1);

		c               = line_info->head[0];
		character       = (Character *)g->font->ascii[c].character;
        cursor->c       = c;
		cursor->offset  = 0;
		cursor->x       = 0;
		cursor->y      += character->height;
		cursor->width   = character->width;

		c              = line_info->head[1];
		character      = (Character *)g->font->ascii[c].character;
        cursor->c      = c;
		cursor->x     += cursor->width;
		cursor->width  = character->width;
		cursor->offset++;
		return;
    }
    else if ((  cursor->x + cursor->width == line_info->line_lenth ||
			    cursor->x == line_info->line_lenth) &&
			    cursor_line < text->last_line_num)
    {
		line_info       = (text_line_t *)text->get_text_line_info(text, cursor_line + 1);
		c               = line_info->head[0];
		character       = (Character *)g->font->ascii[c].character;
        cursor->c       = c;
		cursor->offset  = 0;
		cursor->x       = 0;
		cursor->y      += character->height;
		cursor->width   = character->width;
		return;
	} else if (cursor->x + cursor->width == line_info->line_lenth &&
			   cursor_line == text->last_line_num)
	{
		dbg_str(DBG_SUC,"move cursor to adding char field");
		c               = ' ';
		character       = (Character *)g->font->ascii[c].character;
		cursor->c       = c;
		cursor->x      += cursor->width;
		cursor->offset++;
	} else {
		dbg_str(DBG_WARNNING,
				"curor x=%d width=%d line_num=%d line_lenth=%d last_line_num=%d",
				cursor->x, cursor->width, cursor_line, line_info->line_lenth, 
				text->last_line_num);
	}

	dbg_str(DBG_DETAIL,
			"offset=%d,cursor_line=%d,last_line_num=%d,char =%c,"
			"x pos=%d,char_width =%d",
			cursor->offset,cursor_line, text->last_line_num, 
			cursor->c, cursor->x, character->width);

    return ;
}

void move_cursor_up(Component *component) 
{
	Text_Wicket *tw        = (Text_Wicket *)component;
    Text *text             = tw->text;
	Window *window         = (Window *)tw->window;
	Graph *g               = ((Window *)window)->graph;
    cursor_t *cursor       = &tw->cursor;
    int component_width    = ((Subject *)component)->width;
	int head_offset, i     = 0, width_sum = 0;
	text_line_t *line_info = NULL;
	Character *character;
    uint16_t cursor_line;
	char c = 0;

	cursor_line = get_row_at_cursor(component);
	if (tw->start_line == cursor_line && cursor_line == 0) {
		return;
	}

	line_info = (text_line_t *)text->get_text_line_info(text, cursor_line - 1);
	/*
	 *dbg_str(DBG_DETAIL,"cursor line=%d, data:%s", cursor_line, line_info->string->value);
	 */

	if (cursor->y >= cursor->height) {
		cursor->y -= cursor->height;

		/*case:upper line at cursor pos has character*/
		if (line_info->line_lenth > cursor->x) {
			/*modulate proper cursor pos*/
			for ( i = 0; width_sum < line_info->line_lenth; i++) { 
				c         = line_info->head[i];
				character = (Character *)g->font->ascii[c].character;

				if (	cursor->x >= width_sum &&
						cursor->x <  character->width  + width_sum)
				{
					cursor->x      = width_sum;
                    cursor->c      = c;
                    cursor->offset = i;
					cursor->width  = character->width;
					cursor->height = character->height;

                    break;
				} else {
					width_sum += character->width;
				}
			}
		}
		/*case:upper line at cursor pos doesn' has character*/ 
		else {
			/*last char is '\n'*/
			if ((c = *line_info->tail) == '\n') { 
				cursor->offset = line_info->tail - line_info->head;
			} else {
				cursor->offset = line_info->tail - line_info->head - 1;
			}
            character      = (Character *)g->font->ascii[c].character;
            cursor->x      = line_info->line_lenth - character->width;
            cursor->c      = c;
            cursor->width  = character->width;
            cursor->height = character->height;

		}

		dbg_str(DBG_DETAIL,
				"offset=%d,cursor_line=%d,last_line_num=%d,char =%c,"
				"x pos=%d,char_width =%d",
				cursor->offset,cursor_line - 1, text->last_line_num, 
				cursor->c, cursor->x, character->width);

	} else { //line down
		cursor_t bak  = *cursor;
		bak.y        += bak.height;
		tw->one_line_down(component, g);
		*cursor       = bak;
		move_cursor_up(component);
		return ;
	}

	return ;
}

void move_cursor_down(Component *component) 
{
	Text_Wicket *tw        = (Text_Wicket *)component;
    Text *text             = tw->text;
	Window *window         = (Window *)tw->window;
	Graph *g               = ((Window *)window)->graph;
	cursor_t *cursor       = &tw->cursor;
    int component_height   = ((Subject *)component)->height;
    int component_width    = ((Subject *)component)->width;
	int head_offset, i     = 0, width_sum = 0;
	char c                 = 0;
	text_line_t *line_info = NULL;
	Character *character;
    uint16_t cursor_line,  last_line_num;

	cursor_line   = get_row_at_cursor(component);
	last_line_num = text->get_line_count(text);

	if (cursor_line == last_line_num) {
		dbg_str(DBG_DETAIL, "move_cursor_down, already last line, "
				"cursor_line =%d, last_line_num =%d", cursor_line, last_line_num);
		return;
	}

	line_info = (text_line_t *)text->get_text_line_info(text, cursor_line + 1);

	if (cursor->y + cursor->height * 2 < component_height) {/*not last line*/
		if (line_info->line_lenth > cursor->x) {/*next line has char at cursor pos*/ 
			cursor->y   += cursor->height;
			for (i = 0; ; i++) {
				c         = line_info->head[i];
				character = (Character *)g->font->ascii[c].character;
				if (	cursor->x >= width_sum &&
						cursor->x < width_sum + character->width)
				{
					cursor->x      = width_sum;
					cursor->c      = c;
					cursor->offset = i;
					cursor->width  = character->width;
					cursor->height = character->height;

					break;
				} else {
					width_sum += character->width;
				}
			}
		}  else {
			if ((c = *line_info->tail) == '\n') {
				cursor->offset = line_info->tail - line_info->head;
			} else {
				cursor->offset = line_info->tail - line_info->head - 1;
			}
			cursor->y      += cursor->height;
            character       = (Character *)g->font->ascii[c].character;
            cursor->x       = line_info->line_lenth - character->width;
            cursor->c       = c;
            cursor->width   = character->width;
            cursor->height  = character->height;

		}

		dbg_str(DBG_DETAIL,
				"offset=%d,cursor_line=%d,last_line_num=%d,char =%c,"
				"x pos=%d,char_width =%d",
				cursor->offset,cursor_line + 1, text->last_line_num, 
				cursor->c, cursor->x, character->width);
	}  else { /*last line*/
		cursor_t bak  = *cursor;
		bak.y        -= bak.height;
		tw->one_line_up(component, g);
		*cursor       = bak;
		move_cursor_down(component);
	}


	return ;
}

static int draw_cursor(Component *component,void *graph)
{
	Text_Wicket *tw   = (Text_Wicket *)component;
	Window *window    = (Window *)tw->window;
	Graph *g          = ((Window *)window)->graph;
    cursor_t *cursor  = &tw->cursor;
    color_t *ft_color = &tw->front_color;
    color_t *bg_color = &tw->background_color;
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
	Text_Wicket *tw   = (Text_Wicket *)component;
	Window *window    = (Window *)tw->window;
	Graph *g          = ((Window *)window)->graph;
    cursor_t *cursor  = &tw->cursor;
    color_t *ft_color = &tw->front_color;
    color_t *bg_color = &tw->background_color;
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
    Text_Wicket *tw  = (Text_Wicket *)component;
	Graph *g         = (Graph *)graph;
    cursor_t *cursor = &tw->cursor;
	Character *character;

	character = (Character *)g->font->ascii[c].character;
    if (character->height == 0) {
        dbg_str(DBG_WARNNING,"text list may have problem, draw id=%d, c=%c", c,c);
		return;
    }

    g->render_write_character(g,cursor->x, cursor->y,character);
    cursor->x      += character->width;
    cursor->width   = character->width;
    cursor->height  = character->height;
    cursor->offset++;

	cursor->c = ' ';

	return 0;
}

static int erase_character(Component *component,char c, void *graph)
{
	Text_Wicket *tw   = (Text_Wicket *)component;
	Window *window    = (Window *)tw->window;
	Graph *g          = ((Window *)window)->graph;
    cursor_t *cursor  = &tw->cursor;
    color_t *ft_color = &tw->front_color;
    color_t *bg_color = &tw->background_color;
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

static int draw_n_lines_of_text(Component *component,int from, int to, void *graph)
{
	Text_Wicket *tw        = (Text_Wicket *)component;
    Text *text             = tw->text;
	Graph *g               = (Graph *)graph;
	Subject *s             = (Subject *)component;
    cursor_t *cursor       = &tw->cursor;
	text_line_t *line_info = NULL;
    int i, j;
    char c;

    cursor->x     = 0;
    cursor->y     = from * tw->char_height;
    cursor->width = 0;

	for (   j = tw->start_line + from;
            cursor->y < ((Subject *)component)->height && j <= tw->start_line + to;
            j++) 
    {
		line_info = (text_line_t *)text->get_text_line_info(text,j);
        if (line_info == NULL) break;

		/*
		 *dbg_str(DBG_DETAIL,
		 *        "draw line=%d, len=%d, cursor->x =%d, cursor->y =%d,str=%s",
		 *        j,
		 *        line_info->tail - line_info->head,
		 *        cursor->x,cursor->y,
		 *        line_info->head);
		 */

		for (i = 0; i < line_info->tail - line_info->head + 1; i++) {
			c = line_info->head[i];
			draw_character(component,c, graph);
		}
        cursor->x       = 0;
		cursor->y      += cursor->height;
	}

	g->render_present(g);
}

static int erase_n_lines_of_text(Component *component,int from, int to, void *graph)
{
	Text_Wicket *tw   = (Text_Wicket *)component;
    Text *text        = tw->text;
	Graph *g          = (Graph *)graph;
	Subject *s        = (Subject *)component;
    color_t *bg_color = &tw->background_color;
    int height        = tw->char_height;

    g->render_set_color(g,bg_color->r, bg_color->g, bg_color->b, bg_color->a);
    /*
     *g->render_fill_rect(g, 0, from * height, s->width, (20) * height);
	 *g->render_present(g);
     *g->render_set_color(g, 50, bg_color->g, bg_color->b, bg_color->a);
     */
    g->render_fill_rect(g, 0, from * height, s->width, (to - from + 1) * height);
}

static uint32_t cursor_timer_callback(uint32_t interval, void* param )
{
	__Timer *timer  = (__Timer *)param;
	Text_Wicket *tw = (Text_Wicket *)timer->opaque;
	Window *window  = (Window *)tw->window;
	Graph *g        = ((Window *)window)->graph;

    if((tw->cursor_count++ % 2) == 0) {
        reverse_cursor((Component *)tw,g);
    } else {
        draw_cursor((Component *)tw,g);
    }

	window->remove_timer(window, timer);
	timer->reuse(timer);
}

static int __construct(Text_Wicket *tw,char *init_str)
{
    allocator_t *allocator = ((Obj *)tw)->allocator;
    cursor_t *cursor       = &tw->cursor;

	dbg_str(DBG_SUC,"tw construct");

    tw->string             = OBJECT_NEW(allocator, String, NULL);
    tw->string->assign(tw->string,global_text);

    tw->text               = OBJECT_NEW(allocator, Text,"");
	tw->text->content      = tw->string->value;
	tw->start_line         = 0;

    tw->front_color.r      = 0;
    tw->front_color.g      = 0;
    tw->front_color.b      = 0;
    tw->front_color.a      = 0xff;

    tw->background_color.r = 0xff;
    tw->background_color.g = 0xff;
    tw->background_color.b = 0xff;
    tw->background_color.a = 0xff;

    tw->cursor_count       = 0;
    cursor->x              = 0;
    cursor->y              = 0;

	return 0;
}

static int __deconstrcut(Text_Wicket *tw)
{
	dbg_str(DBG_SUC,"tw deconstruct");

    object_destroy(tw->string);
    object_destroy(tw->text);
	if (tw->timer) {
		object_destroy(tw->timer);
	}

	return 0;
}

static int __set(Text_Wicket *tw, char *attrib, void *value)
{
	if (strcmp(attrib, "set") == 0) {
		tw->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		tw->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		tw->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		tw->deconstruct = value;
	}
	/*vitual methods*/
	else if (strcmp(attrib, "draw") == 0) {
		tw->draw = value;
	} else if (strcmp(attrib, "load_resources") == 0) {
		tw->load_resources = value;
	} else if (strcmp(attrib, "text_key_input") == 0) {
		tw->text_key_input = value;
	} else if (strcmp(attrib, "backspace_key_input") == 0) {
		tw->backspace_key_input = value;
	} else if (strcmp(attrib, "up_key_down") == 0) {
		tw->up_key_down = value;
	} else if (strcmp(attrib, "down_key_down") == 0) {
		tw->down_key_down = value;
	} else if (strcmp(attrib, "left_key_down") == 0) {
		tw->left_key_down = value;
	} else if (strcmp(attrib, "right_key_down") == 0) {
		tw->right_key_down = value;
	} else if (strcmp(attrib, "pageup_key_down") == 0) {
		tw->pageup_key_down = value;
	} else if (strcmp(attrib, "pagedown_key_down") == 0) {
		tw->pagedown_key_down = value;
	} else if (strcmp(attrib, "one_line_up") == 0) {
		tw->one_line_up = value;
	} else if (strcmp(attrib, "one_line_down") == 0) {
		tw->one_line_down = value;
	}
	/*attribs*/
	else if (strcmp(attrib, "name") == 0) {
        strncpy(tw->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"tw set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Text_Wicket *obj, char *attrib)
{
    if (strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"text_wicket get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __load_resources(Component *component,void *window)
{
	Graph *g        = ((Window *)window)->graph;
	Text_Wicket *tw = (Text_Wicket *)component;
    Text *text      = tw->text;
	Character *character;

	dbg_str(DBG_SUC,"%s load load_resources",component->name);

    tw->window          = window;

	g->font->load_ascii_character(g->font,g);
	text->last_line_num = text->write_text(text,0, text->content, g->font) - 1;
	/*
	 *tw->text->line_info->for_each(tw->text->line_info, print_line_info);
	 */

	tw->timer           = ((Window *)window)->create_timer(window);
	tw->timer->opaque   = component;
	tw->timer->set_timer(tw->timer, 1 * 500, cursor_timer_callback); 

	character           = (Character *)g->font->ascii['i'].character;
	tw->char_min_width  = character->width;
    tw->char_height     = character->height;
	tw->cursor.height   = character->height;

	dbg_str(DBG_DETAIL,"cursor height =%d",tw->cursor.height);

	return 0;
}

static int __unload_resources(Component *component,void *window)
{
    //...........
}

static int __draw(Component *component, void *graph)
{
	Text_Wicket *tw        = (Text_Wicket *)component;
    Text *text             = tw->text;
	Graph *g               = (Graph *)graph;
	Subject *s             = (Subject *)component;
    cursor_t *cursor       = &tw->cursor;
	text_line_t *line_info = NULL;
    int i, j;
    char c, c_bak;
    int width_bak;

    /*
	 *dbg_str(DBG_DETAIL,"%s draw", ((Obj *)component)->name);
     */
	g->render_set_color(g,0xff,0xff,0xff,0xff);
	g->render_clear(g);

	g->render_draw_rect(g,s->x,s->y,s->width,s->height);

	cursor->x = 0; cursor->y = 0; cursor->width = 0; 

	for (j = tw->start_line; cursor->y < ((Subject *)component)->height; j++) {
		line_info = (text_line_t *)text->get_text_line_info(text,j);
        if (line_info == NULL) break;

        /*
         *dbg_str(DBG_DETAIL,"draw line=%d, len=%d, cursor->x =%d, cursor->y =%d,str=%s",
         *j, line_info->tail - line_info->head, cursor->x,cursor->y,  line_info->head);
         */
		for (i = 0; i < line_info->tail - line_info->head + 1; i++) {
			c = line_info->head[i];
			draw_character(component,c, graph);

            if(i == 0 && j == tw->start_line) { /*bak cursor info of first char of the screen*/
                c_bak     = line_info->head[0];
                width_bak = cursor->width;
            }
		}
        cursor->x       = 0;
		cursor->y      += cursor->height;
	}

    cursor->x      = 0;
    cursor->y      = 0;
    cursor->offset = 0;
    cursor->c      = c_bak;
    cursor->width  = width_bak;

	g->render_present(g);
}

static int __text_key_input(Component *component,char c, void *graph)
{
	Graph *g                 = (Graph *)graph;
    Text_Wicket *tw          = (Text_Wicket *)component;
    Text *text               = tw->text;
    cursor_t *cursor         = &tw->cursor, cursor_bak;
    int disturbed_line_count = 0;
	Character *character;
    uint16_t cursor_line;
    int from, to;
    int line_count_of_a_screen;

    line_count_of_a_screen = ((Subject *)component)->height / tw->char_height;
	cursor_line            = get_row_at_cursor(component);
    disturbed_line_count   = text->write_char(text,cursor_line ,
											  cursor->offset,
											  cursor->x, 
											  c, g->font);

	character              = (Character *)g->font->ascii[c].character;
	cursor->c              = c;
	cursor->width          = character->width;
    from                   = cursor->y / cursor->height;
    to                     = from + disturbed_line_count - 1;

    if(to > line_count_of_a_screen - 1) {
        to = line_count_of_a_screen - 1;
    }

	move_cursor_right(component);

	cursor_bak             = *cursor;

    /*draw lines disturbed by writing a char*/
    erase_n_lines_of_text(component,from, to, graph);
    draw_n_lines_of_text(component,from, to, g);

	*cursor                = cursor_bak;
    draw_cursor(component,g);

    return 0;
}

static int __backspace_key_input(Component *component,void *graph)
{
	Graph *g                 = (Graph *)graph;
    Text_Wicket *tw          = (Text_Wicket *)component;
    Text *text               = tw->text;
    cursor_t *cursor         = &tw->cursor, cursor_bak;
    int disturbed_line_count = 0;
	Character *character;
    uint16_t cursor_line;
    char c;
    int ret;
    int from, to;
    int line_count_of_a_screen;

	dbg_str(DBG_DETAIL,"backspace_key_input");

    line_count_of_a_screen = ((Subject *)component)->height / tw->char_height;
    c                      = cursor->c;

	ret                    = move_cursor_left(component);
    if (ret < 1) { return; }

	cursor_line            = get_row_at_cursor(component);
    disturbed_line_count   = text->delete_char(text,cursor_line ,
											   cursor->offset,
											   cursor->x, 
											   g->font);

	character              = (Character *)g->font->ascii[c].character;
	cursor->c              = c;
	cursor->width          = character->width;
    from                   = cursor->y / cursor->height;
    to                     = from + disturbed_line_count - 1;

    if(to > line_count_of_a_screen - 1) {
        to = line_count_of_a_screen - 1;
    }

	cursor_bak             = *cursor;

    erase_n_lines_of_text(component,from,to, g);
    draw_n_lines_of_text(component,from, to, g);

	*cursor                = cursor_bak;
    draw_cursor(component,g);

    return 0;
}

static int __up_key_down(Component *component,void *graph)
{
	Graph *g          = (Graph *)graph;
	Text_Wicket *tw   = (Text_Wicket *)component;
    cursor_t *cursor  = &tw->cursor;
    color_t *bg_color = &tw->background_color;

    reverse_cursor(component,g);
	move_cursor_up(component);
    draw_cursor(component,g);

    tw->cursor_count  = 0;

    return 0;
}

static int __down_key_down(Component *component,void *graph)
{
	Graph *g         = (Graph *)graph;
	Text_Wicket *tw  = (Text_Wicket *)component;
    cursor_t *cursor = &tw->cursor;

    reverse_cursor(component,g);
	move_cursor_down(component);
    draw_cursor(component,g);

    tw->cursor_count = 0;

    return 0;
}

static int __left_key_down(Component *component,void *graph)
{
	Graph *g         = (Graph *)graph;
	Text_Wicket *tw  = (Text_Wicket *)component;
    cursor_t *cursor = &tw->cursor;

	/*
	 *dbg_str(DBG_DETAIL,"left_key_down");
	 */

    reverse_cursor(component,g);
	move_cursor_left(component);
    draw_cursor(component,g);

    tw->cursor_count = 0;
}

static int __right_key_down(Component *component,void *graph)
{
	Graph *g         = (Graph *)graph;
	Text_Wicket *tw  = (Text_Wicket *)component;
    cursor_t *cursor = &tw->cursor;

	/*
	 *dbg_str(DBG_DETAIL,"right_key_down");
	 */

    reverse_cursor(component,g);
	move_cursor_right(component);
    draw_cursor(component,g);

    tw->cursor_count = 0;
}

static int __pgup_key_down(Component *component,void *graph)
{
	Graph *g               = (Graph *)graph;
    Text_Wicket *tw        = (Text_Wicket *)component;
    Text *text             = tw->text;
	text_line_t *line_info = NULL;
    cursor_t *cursor       = &tw->cursor, cursor_bak;
    int line_count_of_a_screen;
    uint16_t cursor_line;
	Character *character;
    char c;

	dbg_str(DBG_DETAIL,"pgup_key_down");
    line_count_of_a_screen = ((Subject *)component)->height / tw->char_height;

    if(tw->start_line - line_count_of_a_screen > 0) {
        tw->start_line -= line_count_of_a_screen;
    } else if (tw->start_line > 0) {
        tw->start_line = 0;
    } else {
        dbg_str(DBG_WARNNING,"pgup_key_down");
        return ;
    }

	cursor->y      = 0;
	cursor->x      = 0;
    cursor->offset = 0;

	tw->draw(component,graph); 
}

static int __pgdown_key_down(Component *component,void *graph)
{
	Graph *g               = (Graph *)graph;
    Text_Wicket *tw        = (Text_Wicket *)component;
    Text *text             = tw->text;
	text_line_t *line_info = NULL;
    cursor_t *cursor       = &tw->cursor, cursor_bak;
    int line_count_of_a_screen;
    uint16_t cursor_line;
	Character *character;
    char c;

	dbg_str(DBG_DETAIL,"pgdown_key_down");
    line_count_of_a_screen = ((Subject *)component)->height / tw->char_height;

    if(tw->start_line + line_count_of_a_screen < text->last_line_num ) {
        tw->start_line += line_count_of_a_screen;
    } else {
        return;
    }

	cursor->y      = 0;
	cursor->x      = 0;
    cursor->offset = 0;
	tw->draw(component,graph); 
}

static int __one_line_up(Component *component,void *graph)
{
	Graph *g         = (Graph *)graph;
    Text_Wicket *tw  = (Text_Wicket *)component;
    cursor_t *cursor = &tw->cursor;

	dbg_str(DBG_DETAIL,"one line up");

	cursor->y        = 0;
	cursor->x        = 0;
	tw->start_line++;
	tw->draw(component,graph); 
}

static int __one_line_down(Component *component,void *graph)
{
    Text_Wicket *tw  = (Text_Wicket *)component;
	Graph *g         = (Graph *)graph;
    cursor_t *cursor = &tw->cursor;

	dbg_str(DBG_DETAIL,"one line down");

	cursor->y        = 0;
	cursor->x        = 0;

	if (tw->start_line) {
		tw->start_line--;
		tw->draw(component,graph); 
	} else if (tw->start_line == 0) {
		tw->draw(component,graph); 
	}

}

static class_info_entry_t text_wicket_class_info[] = {
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
REGISTER_CLASS("Text_Wicket",text_wicket_class_info);

char *gen_text_wicket_setting_str()
{
    cjson_t *root,*b, *c, *e, *s;
    char *set_str;

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Text_Wicket", b = cjson_create_object());{
            cjson_add_item_to_object(root, "Component", c = cjson_create_object());{
                cjson_add_item_to_object(root, "Container", e = cjson_create_object());{
                    cjson_add_item_to_object(e, "Subject", s = cjson_create_object());{
                        cjson_add_number_to_object(s, "x", 0);
                        cjson_add_number_to_object(s, "y", 0);
                        cjson_add_number_to_object(s, "width", 600);
                        cjson_add_number_to_object(s, "height", 600);
                    }
                }
				cjson_add_string_to_object(c, "name", "text_wicket");
            }
        }
    }
    set_str = cjson_print(root);

    return set_str;
}
void test_ui_text_wicket()
{
    Window *window;
    Container *container;
	Graph *g;
    Subject *subject;
	__Event *event;
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

    set_str   = gen_text_wicket_setting_str();
    subject   = OBJECT_NEW(allocator, Text_Wicket,set_str);

    object_dump(subject, "Text_Wicket", buf, 2048);
    dbg_str(DBG_DETAIL,"Text_Wicket dump: %s",buf);

    container->add_component(container,subject);
	/*
     *container->search_component(container,"text_wicket");
	 */
	dbg_str(DBG_DETAIL,"window container :%p",container);

	window->load_resources(window);
	window->update_window(window);

    event->poll_event(event, window);

    object_destroy(window);

    free(set_str);
}
