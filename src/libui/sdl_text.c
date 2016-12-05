/**
 * @file text_sdl.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-04
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/sdl_text.h>
#include <libui/sdl_window.h>

static int __construct(Text *text,char *init_str)
{
	dbg_str(OBJ_DETAIL,"text construct, text addr:%p",text);

	return 0;
}

static int __deconstrcut(Text *text)
{
	SDL_Text *i = (SDL_Text *)text;
	dbg_str(OBJ_DETAIL,"text deconstruct,text addr:%p",text);

	if(i->surface != NULL){
		SDL_FreeSurface(i->surface);
	}

	return 0;
}

static int __set(Text *text, char *attrib, void *value)
{
	SDL_Text *i = (SDL_Text *)text;

	if(strcmp(attrib, "set") == 0) {
		i->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		i->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		i->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		i->deconstruct = value;
	} else if(strcmp(attrib, "load_text") == 0) {
		i->load_text = value;
	} else {
		dbg_str(OBJ_WARNNING,"text set,  \"%s\" setting is not support",attrib);
	}

	return 0;
}

static void * __get(Text *text, char *attrib)
{
    if(strcmp(attrib, "x") == 0){ 
    } else {
        dbg_str(OBJ_WARNNING,"text get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __load_text(Text *text,void *font)
{
	SDL_Text *t = (SDL_Text *)text;
	SDL_Font *f = (SDL_Font *)font;
	SDL_Color textColor = { 0, 0, 0, 0xFF };

	dbg_str(DBG_SUC,"SDL_Text load text");
	t->surface = TTF_RenderText_Solid(f->ttf_font, text->content->value, textColor ); 
}

static class_info_entry_t text_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Text","text",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","load_text",__load_text,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("SDL_Text",text_class_info);

static char *gen_window_setting_str()
{
    cjson_t *root,*w, *c, *e, *s;
    char *set_str;

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Window", w = cjson_create_object());{
            cjson_add_item_to_object(root, "Component", c = cjson_create_object());{
                cjson_add_item_to_object(root, "Container", e = cjson_create_object());{
                    cjson_add_item_to_object(e, "Subject", s = cjson_create_object());{
                        cjson_add_number_to_object(s, "x", 1);
                        cjson_add_number_to_object(s, "y", 25);
                        cjson_add_number_to_object(s, "width", 5);
                        cjson_add_number_to_object(s, "height", 125);
                        cjson_add_number_to_object(s, "x_speed", 1.2);
                        cjson_add_number_to_object(s, "y_speed", 2.3);
                    }
                    cjson_add_string_to_object(e, "name", "Container");
                }
                cjson_add_string_to_object(c, "name", "Component");
            }
            cjson_add_string_to_object(w, "name", "Window");
			cjson_add_number_to_object(w, "graph_type", 1);
			cjson_add_number_to_object(w, "screen_width", 640);
			cjson_add_number_to_object(w, "screen_height", 480);
        }
    }
    set_str = cjson_print(root);

    return set_str;
}

void test_obj_sdl_text()
{
    Window *window;
	Graph *g;
	Text *text;
	Font *font;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    char buf[2048];

    set_str = gen_window_setting_str();

    window  = OBJECT_NEW(allocator, SDL_Win,set_str);
	g       = window->graph;

    object_dump(window, "SDL_Win", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

	/*
     *font = OBJECT_NEW(allocator, SDL_Font,"");
	 *font->load_font(font);
	 */

    text = OBJECT_NEW(allocator, SDL_Text,"");
	text->content->assign(text->content,"hello world");
	g->render_load_text(g,text,window->font, 0,0,0,0xff);
	g->render_write_text(g,0,0,text);

	text->content->assign(text->content,"a");
	g->render_load_text(g,text,window->font, 0,0,0,0xff);
	g->render_write_text(g,0,33,text);

	text->content->assign(text->content,"b");
	g->render_load_text(g,text,window->font, 0,0,0,0xff);
	g->render_write_text(g,0,66,text);

	text->content->assign(text->content,"i");
	g->render_load_text(g,text,window->font, 0,0,0,0xff);
	g->render_write_text(g,0,99,text);

	g->render_present(g);

	pause();

    object_destroy(window);

    free(set_str);

}


