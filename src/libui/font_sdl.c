/**
 * @file font.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-03
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/font_sdl.h>

static int __construct(Font *font,char *init_str)
{
	SDL_Font *f = (SDL_Font *)font;

	dbg_str(OBJ_DETAIL,"font construct, font addr:%p",font);

	if(TTF_Init() == -1){
		dbg_str(DBG_WARNNING,"TTF_Init error");
		return -1;
	}

	return 0;
}

static int __deconstrcut(Font *font)
{
	SDL_Font *f = (SDL_Font *)font;
	dbg_str(OBJ_DETAIL,"font deconstruct,font addr:%p",font);

	if(f->ttf_font != NULL) {
		TTF_CloseFont(f->ttf_font);
	}

	TTF_Quit();
	return 0;
}

static int __set(Font *font, char *attrib, void *value)
{
	SDL_Font *i = (SDL_Font *)font;

	if(strcmp(attrib, "set") == 0) {
		i->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		i->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		i->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		i->deconstruct = value;
	} else if(strcmp(attrib, "load_font") == 0) {
		i->load_font = value;
	} else {
		dbg_str(OBJ_WARNNING,"font set,  \"%s\" setting is not support",attrib);
	}

	return 0;
}

static void * __get(Font *font, char *attrib)
{
    if(strcmp(attrib, "x") == 0){ 
    } else {
        dbg_str(OBJ_WARNNING,"font get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __load_font(Font *font)
{
	SDL_Font *f = (SDL_Font *)font;

	f->ttf_font = TTF_OpenFont( "./bin/Vera.ttf", 28 );
	if(f->ttf_font == NULL) {
	} else {
		dbg_str(DBG_SUC,"SDL_Graph load font");
	}
}

static class_info_entry_t font_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Font","font",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","load_font",__load_font,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("SDL_Font",font_class_info);

void test_obj_sdl_font()
{
	SDL_Font *font;
	allocator_t *allocator = allocator_get_default_alloc();

    font = OBJECT_NEW(allocator, SDL_Font,"");
}


