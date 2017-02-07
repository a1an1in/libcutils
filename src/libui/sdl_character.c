/**
 * @file character_sdl.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-04
 */
#include <stdio.h>
#include <unistd.h>
#include <libdbg/debug.h>
#include <libui/sdl_character.h>
#include <libui/sdl_window.h>

static int __construct(Character *character,char *init_str)
{
    dbg_str(OBJ_DETAIL,"character construct, character addr:%p",character);

    return 0;
}

static int __deconstrcut(Character *character)
{
    Sdl_Character *c = (Sdl_Character *)character;
    dbg_str(OBJ_DETAIL,"character deconstruct,character addr:%p",character);

    if(c->surface != NULL){
        SDL_FreeSurface(c->surface);
    }
    if(c->texture != NULL) {
        SDL_DestroyTexture(c->texture);
    }

    return 0;
}

static int __set(Character *character, char *attrib, void *value)
{
    Sdl_Character *c = (Sdl_Character *)character;

    if(strcmp(attrib, "set") == 0) {
        c->set = value;
    } else if(strcmp(attrib, "get") == 0) {
        c->get = value;
    } else if(strcmp(attrib, "construct") == 0) {
        c->construct = value;
    } else if(strcmp(attrib, "deconstruct") == 0) {
        c->deconstruct = value;
    } else if(strcmp(attrib, "load_character") == 0) {
        c->load_character = value;
    } else {
        dbg_str(OBJ_WARNNING,"character set,  \"%s\" setting is not support",attrib);
    }

    return 0;
}

static void * __get(Character *character, char *attrib)
{
    if(strcmp(attrib, "") == 0){ 
    } else {
        dbg_str(OBJ_WARNNING,"character get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __load_character(Character *character,void *graph)
{
}

static class_info_entry_t character_class_info[] = {
    [0 ] = {ENTRY_TYPE_OBJ,"Character","character",NULL,sizeof(void *)},
    [1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
    [2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
    [3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
    [4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
    [5 ] = {ENTRY_TYPE_FUNC_POINTER,"","load_character",__load_character,sizeof(void *)},
    [6 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Sdl_Character",character_class_info);

void test_obj_sdl_character()
{
    Window *window;
    Graph *g;
    Character *character;
    Font *font;
    allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    char buf[2048];

    set_str = gen_window_setting_str();

    window  = OBJECT_NEW(allocator, Sdl_Window,set_str);
    g       = window->graph;

    object_dump(window, "Sdl_Window", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

    character = g->render_load_character(g,'a',window->font, 0,0,0,0xff);
    g->render_write_character(g,0,33,character);

    g->render_present(g);
    g->render_unload_character(g, character);

    pause();

    object_destroy(window);

    free(set_str);

}


