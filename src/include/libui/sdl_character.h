#ifndef __SDL_CHARACTER_H__
#define __SDL_CHARACTER_H__

#include <libobject/obj.h>
#include <libobject/string.h>
#include <libui/character.h>
#include <libui/sdl_font.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct sdl_character_s Sdl_Character;

struct sdl_character_s{
	Character character;

	/*normal methods*/
	int (*construct)(Character *character,char *init_str);
	int (*deconstruct)(Character *character);
	int (*set)(Character *character, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods*/
	int (*load_character)(Character *character,void *graph);

	/*attribs*/
	SDL_Surface* surface;
	SDL_Texture* texture;
	int width;
	int height;
};

#endif
