/**
 * @file font.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-03
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/sdl_font.h>
#include <libui/graph.h>
#include <libui/character.h>

static int __construct(Font *font,char *init_str)
{
	Sdl_Font *f = (Sdl_Font *)font;

	dbg_str(OBJ_DETAIL,"font construct, font addr:%p",font);

	if(TTF_Init() == -1){
		dbg_str(DBG_WARNNING,"TTF_Init error");
		return -1;
	}

	return 0;
}

static int __deconstrcut(Font *font)
{
	Sdl_Font *f = (Sdl_Font *)font;
	dbg_str(OBJ_DETAIL,"font deconstruct,font addr:%p",font);

	if(f->ttf_font != NULL) {
		TTF_CloseFont(f->ttf_font);
	}

	TTF_Quit();
	return 0;
}

static int __set(Font *font, char *attrib, void *value)
{
	Sdl_Font *f = (Sdl_Font *)font;

	if(strcmp(attrib, "set") == 0) {
		f->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		f->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		f->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		f->deconstruct = value;
	}
    else if(strcmp(attrib, "load_font") == 0) {
		f->load_font = value;
    } else if(strcmp(attrib, "unload_font") == 0) {
		f->unload_font = value;
	} else if(strcmp(attrib, "load_ascii_character") == 0) {
		f->load_ascii_character = value;
	} else if(strcmp(attrib, "unload_ascii_character") == 0) {
		f->unload_ascii_character = value;
	} else if(strcmp(attrib, "get_character_width") == 0) {
		f->get_character_width = value;
	} else if(strcmp(attrib, "get_character_height") == 0) {
		f->get_character_height = value;
	}
    else {
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
	Sdl_Font *f = (Sdl_Font *)font;

    f->ttf_font = TTF_OpenFont( "./bin/Vera.ttf", 15 );
    /*
     *f->ttf_font = TTF_OpenFont( "./bin/UbuntuMono-R.ttf", 18 );
     */

     /*
      *f->ttf_font = TTF_OpenFont( "./bin/TIMES.TTF", 20 );
      */
     /*
      *f->ttf_font = TTF_OpenFont( "./bin/ARIAL.TTF", 15 );
      */
 
    /*
     *TTF_SetFontStyle(f->ttf_font, TTF_STYLE_BOLD);
     */

	if(f->ttf_font == NULL) {
	} else {
		dbg_str(DBG_DETAIL,"Sdl_Graph load font");
	}
}

static int __unload_font(Font *font)
{
	Sdl_Font *f = (Sdl_Font *)font;

    //...........
}

static int __load_ascii_character(Font *font,void *graph)
{
	int i;                                                                                                               
	Graph *g = (Graph *)graph;
	Character *character;   
	ascii_character_t *ascii = font->ascii;  

	dbg_str(DBG_DETAIL,"load asscii info"); 
	for( i = 0; i < 128; i++) {  
		character = g->render_load_character(g,i,font,0,0,0,0xff); 
		ascii[i].height = character->height;
		ascii[i].width  = character->width;
		ascii[i].character = character;
	}      
    
    return 0;
}

static int __unload_ascii_character(Font *font,void *graph)
{
	int i;                                                                                                               
	ascii_character_t *ascii = font->ascii;  

	dbg_str(DBG_DETAIL,"unload asscii info"); 

	for( i = 0; i < 128; i++) {  
        object_destroy(ascii[i].character);   
	}      

    return 0;
}
static int __get_character_width(Font *font, char c)
{
	return font->ascii[c].width;
}

static int __get_character_height(Font *font, char c)
{
	return font->ascii[c].height;
}

static class_info_entry_t font_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Font","font",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","load_font",__load_font,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_FUNC_POINTER,"","unload_font",__unload_font,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_FUNC_POINTER,"","load_ascii_character",__load_ascii_character,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_FUNC_POINTER,"","unload_ascii_character",__unload_ascii_character,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_FUNC_POINTER,"","get_character_width",__get_character_width,sizeof(void *)},
	[10] = {ENTRY_TYPE_FUNC_POINTER,"","get_character_height",__get_character_height,sizeof(void *)},
	[11] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Sdl_Font",font_class_info);

void test_obj_sdl_font()
{
	Font *font;
	allocator_t *allocator = allocator_get_default_alloc();
	int w, h;

    font = OBJECT_NEW(allocator, Sdl_Font,"");
	w = font->get_character_width(font,'a');
	h = font->get_character_height(font,'a');

	dbg_str(DBG_DETAIL,"a's w=%d h=%d", w, h);

}


