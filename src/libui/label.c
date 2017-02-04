/**
 * @file label.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/label.h>
#include <libui/sdl_window.h>
#include <libui/character.h>
#include <libui/timer.h>

static int draw_character(Component *component,char c, void *graph)
{
    Label *label     = (Label *)component;
	Graph *g         = (Graph *)graph;
    cursor_t *cursor = &label->cursor;
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

static int __construct(Label *label,char *init_str)
{
    allocator_t *allocator = ((Obj *)label)->allocator;
    cursor_t *cursor       = &label->cursor;

	dbg_str(DBG_SUC,"label construct");

    label->string             = OBJECT_NEW(allocator, String, NULL);
    label->string->assign(label->string,"button1");

    label->front_color.r      = 0;
    label->front_color.g      = 0;
    label->front_color.b      = 0;
    label->front_color.a      = 0xff;

    label->background_color.r = 0xff;
    label->background_color.g = 0xff;
    label->background_color.b = 0xff;
    label->background_color.a = 0xff;

    cursor->x              = 0;
    cursor->y              = 0;

	return 0;
}

static int __deconstrcut(Label *label)
{
	dbg_str(DBG_SUC,"label deconstruct");

    object_destroy(label->string);

	return 0;
}

static int __set(Label *label, char *attrib, void *value)
{
	if (strcmp(attrib, "set") == 0) {
		label->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		label->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		label->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		label->deconstruct = value;
	}
	/*vitual methods*/
	else if (strcmp(attrib, "draw") == 0) {
		label->draw = value;
	} else if (strcmp(attrib, "load_resources") == 0) {
		label->load_resources = value;
	} else if (strcmp(attrib, "unload_resources") == 0) {
		label->unload_resources = value;
	}
	/*attribs*/
	else if (strcmp(attrib, "name") == 0) {
        strncpy(label->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"label set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Label *obj, char *attrib)
{
    if (strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"label get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __load_resources(Component *component,void *window)
{
	Graph *g     = ((Window *)window)->graph;
	Label *label = (Label *)component;
	Character *character;

	dbg_str(DBG_SUC,"%s load resources",component->name);

    label->window          = window;

	character             = (Character *)g->font->ascii['i'].character;
	label->char_min_width = character->width;
    label->char_height    = character->height;
	label->cursor.height  = character->height;

	dbg_str(DBG_DETAIL,"cursor height =%d",label->cursor.height);

	return 0;
}

static int __unload_resources(Component *component,void *window)
{
	Graph *g     = ((Window *)window)->graph;

    /*
	 *g->font->unload_ascii_character(g->font,g);
     */
}

static int __draw(Component *component, void *graph)
{
	Label *label             = (Label *)component;
	Graph *g                 = (Graph *)graph;
	Subject *s               = (Subject *)component;
    cursor_t *cursor         = &label->cursor;
    unsigned int i, j, str_len, count = 0;
    char c;

	g->render_clear(g);
	g->render_set_color(g,0,0,0,0xff);

	g->render_draw_rect(g,s->x,s->y,s->width,s->height);

	cursor->x = s->x; cursor->y = s->y; cursor->width = 0; 

    str_len = strlen(label->string->value);

	for (j = 0; cursor->y + cursor->height < ((Subject *)component)->height + s->y; j++) {
		for (i = 0; cursor->x + cursor->width < ((Subject *)component)->width + s->x; i++) {
			c = label->string->at(label->string, count++);
			draw_character(component,c, graph);
            if(count  == str_len){
                dbg_str(DBG_DETAIL,"count =%d",count);
                goto end;
            } 
        }
        cursor->x  = s->x;
		cursor->y += cursor->height;
    }

end:
    g->render_present(g);
}

static class_info_entry_t label_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Component","component",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","load_resources",__load_resources,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_FUNC_POINTER,"","unload_resources",__unload_resources,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_FUNC_POINTER,"","draw",__draw,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[9 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Label",label_class_info);

char *gen_label_setting_str()
{
    cjson_t *root,*b, *c, *e, *s;
    char *set_str;

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Label", b = cjson_create_object());{
            cjson_add_item_to_object(b, "Component", c = cjson_create_object());{
                cjson_add_item_to_object(c, "Container", e = cjson_create_object());{
                    cjson_add_item_to_object(e, "Subject", s = cjson_create_object());{
                        cjson_add_number_to_object(s, "x", 10);
                        cjson_add_number_to_object(s, "y", 20);
                        cjson_add_number_to_object(s, "width", 100);
                        cjson_add_number_to_object(s, "height", 50);
                    }
                }
				cjson_add_string_to_object(c, "name", "label");
            }
        }
    }
    set_str = cjson_print(root);

    return set_str;
}

void test_ui_label()
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

    set_str   = gen_label_setting_str();
    subject   = OBJECT_NEW(allocator, Label,set_str);

    object_dump(subject, "Label", buf, 2048);
    dbg_str(DBG_DETAIL,"Label dump: %s",buf);

    container->add_component(container,subject);

	dbg_str(DBG_DETAIL,"window container :%p",container);

	window->load_resources(window);
	window->update_window(window);

    event->poll_event(event, window);

    object_destroy(window);

    free(set_str);
}
