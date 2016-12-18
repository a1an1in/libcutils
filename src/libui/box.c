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

extern void print_line_info(Iterator *iter);

char *global_text =
"A newly published book on the documents of a late United States diplomat, who "
"was also a scholar on Tibet, has provided conclusive evidence that the envoy in t"
"he early 20th century considered Tibet to be an inseparable part of China. Select"
"ed Documents Relating to Tibet from William W. Rockhill Papers, compiled by Cheng Long, a former associate professor at Beijing Language and Culture University, was published recently by China Intercontinental Press.  William Rockhill (1854-1914), a US diplomat, explorer and scholar on Tibet, was the author of the United States' Open Door Policy for China and the author of several books on Tibetan studies.  Rockhill several times pointed out to the US public that Tibet is an inseparable part of China, and he introduced his position to president Theodore Roosevelt, Cheng said.\n Cheng said he began to pay attention to Rockhill while he was teaching at the University of South Carolina in 2008, after US students kept asking questions about Tibet.  So I came up with an idea - to tell the history and culture of Tibet by using Western historical documents, which are more convincing to them, he said.  From 2008 to early this year, Cheng searched for documents at such places as Harvard and Yale universities, the Library of Congress and the US National Archives, and he collected abundant materials from Rockhill related to Tibet.  According to the US State Department website, Rockhill, who was born in Philadelphia, was appointed as the third assistant secretary of state in April 1894. He was appointed US ambassador to China in 1905 and held the position until 1909.  Rockhill was also a famous scholar of Tibet and visited the region twice. In 1908, he met several times with the 13th Dalai Lama, with whom he kept in touch through letters.  Rockhill was appointed to an unpaid post at the US legation in Beijing in 1883 on his first trip to China.\nHe intended to perfect his spoken Tibetan and Chinese and to travel to Lhasa, according to William Woodville Rockhill Scholar-Diplomat of the Tibetan Highlands, a book by the late Kenneth Wimmel, who was a US foreign affairs officer. Cheng said that Rockhill had the habit of keeping copies of important files and that these materials are well preserved.";

static int __construct(Box *box,char *init_str)
{
    allocator_t *allocator = ((Obj *)box)->allocator;
	dbg_str(DBG_SUC,"box construct");
    box->string = OBJECT_NEW(allocator, String, NULL);

    box->string->assign(box->string,global_text);
    box->text = OBJECT_NEW(allocator, Text,"");
	box->text->content = box->string->value;
	box->start_line = 1;


	return 0;
}

static int __deconstrcut(Box *box)
{
	dbg_str(DBG_SUC,"box deconstruct");
    object_destroy(box->string);
    object_destroy(box->text);

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
	} else if(strcmp(attrib, "draw") == 0) {
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
	} else if(strcmp(attrib, "name") == 0) {
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

static int __load_resources(Component *component,void *graph)
{
}

static int write_character(Component *component,char c, void *graph)
{
	Graph *g = (Graph *)graph;
	Character *character;
    Box *b = (Box *)component;

	character = (Character *)g->render_load_character(g,(uint32_t)c,g->font, 0,0,0,0xff);
    if(b->x + character->width > ((Subject *)component)->width) {
        b->x = 0;
        b->max_height = 0;
        b->y += character->height;
    }
    if(b->max_height < b->y) {
        b->max_height = b->y;
    }
	g->render_write_character(g,b->x,b->y,character);
	b->x += character->width;

    object_destroy(character);

}

static int __draw(Component *component, void *graph)
{
	Graph *g = (Graph *)graph;
	Subject *s = (Subject *)component;
	Box *b = (Box *)component;
    int i;
    char c;
	int start;
	static int count = 0;

	dbg_str(DBG_DETAIL,"%s draw", ((Obj *)component)->name);
	g->render_set_color(g,0xff,0xff,0xff,0xff);
	g->render_clear(g);

	count++;
	if(count == 1) {
		g->font->load_ascii_info(g->font,g);
		b->text->parse_text(b->text, 0, g->font);
		b->text->line_info->for_each(b->text->line_info, print_line_info);
	}
	start = b->text->get_head_offset_of_line(b->text, b->start_line);
	dbg_str(DBG_IMPORTANT,"start line =%d start offset =%d",b->start_line, start);
	if(start < 0) {
		dbg_str(DBG_WARNNING,"get head offset of one line err");
		return -1;
	}

#if 1
	/*
	 *g->render_set_color(g,0x0,0x0,0x0,0xff);
	 */
	g->render_draw_rect(g,s->x,s->y,s->width,s->height);

	dbg_str(DBG_DETAIL,"draw x=%d, y=%d", b->x, b->y);
	char buf[10] = {0};
	memcpy(buf, b->string->value + start, 10);
	dbg_str(DBG_DETAIL,"%s", buf);

    for(i = 0; i < strlen(b->string->value + start); i++) {
        c = b->string->value[i + start];
        write_character(component,c, graph);
        if(b->y > ((Subject *)component)->height ){
            dbg_str(DBG_DETAIL,"box y =%d , subject height =%d", b->y, ((Subject *)component)->height);
            break;
        }
    }

	g->render_present(g);
#else
	b->text->line_info->for_each(b->text->line_info, print_line_info);
#endif

}

static int __text_key_input(Component *component,char c, void *graph)
{
	Graph *g = (Graph *)graph;
	Character *character;
    Box *b = (Box *)component;

	dbg_str(DBG_DETAIL,"text input");

	character = (Character *)g->render_load_character(g,(uint32_t)c,g->font, 0,0,0,0xff);
    if(b->x + character->width > ((Subject *)component)->width) {
        b->x = 0;
        b->max_height = 0;
        b->y += character->height;
    }
    if(b->max_height < b->y) {
        b->max_height = b->y;
    }
	g->render_write_character(g,b->x,b->max_height,character);
	b->x += character->width;
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
	Graph *g = (Graph *)graph;
    Box *b = (Box *)component;
	dbg_str(DBG_DETAIL,"up_key_down");

	b->y = 0;
	b->x = 0;
	b->start_line++;
	b->draw(component,graph); 
	dbg_str(DBG_DETAIL,"start line=%d",b->start_line);
}

static int __down_key_down(Component *component,void *graph)
{
    Box *b = (Box *)component;
	Graph *g = (Graph *)graph;
	dbg_str(DBG_DETAIL,"down_key_down");
	b->y = 0;
	b->x = 0;
	if(b->start_line - 1) {
		b->start_line--;
		b->draw(component,graph); 
	} else if(b->start_line == 1) {
		b->draw(component,graph); 
	}
	dbg_str(DBG_DETAIL,"start line=%d",b->start_line);
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
	[15] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[16] = {ENTRY_TYPE_END},

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

	window->update_window(window);

    event->poll_event(event, window);

    object_destroy(window);

    free(set_str);

}


