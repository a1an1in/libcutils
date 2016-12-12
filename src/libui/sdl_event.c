/**
 * @file event_sdl.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 
 * @date 2016-12-04
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/sdl_event.h>
#include <libui/sdl_window.h>

static void
print_string(char **text, size_t *maxlen, const char *fmt, ...)
{
    int len;
    va_list ap;

    va_start(ap, fmt);
    len = SDL_vsnprintf(*text, *maxlen, fmt, ap);
    if (len > 0) {
        *text += len;
        if ( ((size_t) len) < *maxlen ) {
            *maxlen -= (size_t) len;
        } else {
            *maxlen = 0;
        }
    }
    va_end(ap);
}

static void
print_modifiers(char **text, size_t *maxlen)
{
    int mod;
    print_string(text, maxlen, " modifiers:");
    mod = SDL_GetModState();
    if (!mod) {
        print_string(text, maxlen, " (none)");
        return;
    }
    if (mod & KMOD_LSHIFT)
        print_string(text, maxlen, " LSHIFT");
    if (mod & KMOD_RSHIFT)
        print_string(text, maxlen, " RSHIFT");
    if (mod & KMOD_LCTRL)
        print_string(text, maxlen, " LCTRL");
    if (mod & KMOD_RCTRL)
        print_string(text, maxlen, " RCTRL");
    if (mod & KMOD_LALT)
        print_string(text, maxlen, " LALT");
    if (mod & KMOD_RALT)
        print_string(text, maxlen, " RALT");
    if (mod & KMOD_LGUI)
        print_string(text, maxlen, " LGUI");
    if (mod & KMOD_RGUI)
        print_string(text, maxlen, " RGUI");
    if (mod & KMOD_NUM)
        print_string(text, maxlen, " NUM");
    if (mod & KMOD_CAPS)
        print_string(text, maxlen, " CAPS");
    if (mod & KMOD_MODE)
        print_string(text, maxlen, " MODE");
}

static void
print_modifier_state()
{
    char message[512];
    char *spot;
    size_t left;

    spot = message;
    left = sizeof(message);

    print_modifiers(&spot, &left);
    dbg_str(DBG_DETAIL,"Initial state:%s", message);
}

static void
print_key(SDL_Keysym * sym, SDL_bool pressed, SDL_bool repeat)
{
    char message[512];
    char *spot;
    size_t left;

    spot = message;
    left = sizeof(message);

    /* Print the keycode, name and state */
    if (sym->sym) {
        print_string(&spot, &left,
                     "Key %s:  scancode %d = %s, keycode 0x%08X = %s ",
                     pressed ? "pressed " : "released",
                     sym->scancode,
                     SDL_GetScancodeName(sym->scancode),
                     sym->sym, SDL_GetKeyName(sym->sym));
    } else {
        print_string(&spot, &left,
                     "Unknown Key (scancode %d = %s) %s ",
                     sym->scancode,
                     SDL_GetScancodeName(sym->scancode),
                     pressed ? "pressed " : "released");
    }
    print_modifiers(&spot, &left);
    if (repeat) {
        print_string(&spot, &left, " (repeat)");
    }
    dbg_str(DBG_DETAIL,"%s", message);
}

static void
print_text(char *eventtype, char *text)
{
    char *spot, expanded[1024];

    expanded[0] = '\0';
    for ( spot = text; *spot; ++spot )
    {
        size_t length = SDL_strlen(expanded);
        SDL_snprintf(expanded + length, sizeof(expanded) - length, "\\x%.2x", (unsigned char)*spot);
    }
    dbg_str(DBG_DETAIL, "%s Text (%s): \"%s%s\"", eventtype, expanded, *text == '"' ? "\\" : "", text);
}

static int __construct(Event *event,char *init_str)
{
	dbg_str(OBJ_DETAIL,"event construct, event addr:%p",event);

	return 0;
}

static int __deconstrcut(Event *event)
{
	Event *i = (Event *)event;
	dbg_str(OBJ_DETAIL,"event deconstruct,event addr:%p",event);


	return 0;
}

static int __set(Event *event, char *attrib, void *value)
{
    Sdl_Event *e = (Sdl_Event *)event;

	if(strcmp(attrib, "set") == 0) {
		e->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		e->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		e->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		e->deconstruct = value;
	} else if(strcmp(attrib, "poll_event") == 0) {
		e->poll_event = value;
	} else {
		dbg_str(OBJ_WARNNING,"event set,  \"%s\" setting is not support",attrib);
	}

	return 0;
}

static void * __get(Event *event, char *attrib)
{
    if(strcmp(attrib, "") == 0){ 
    } else {
        dbg_str(OBJ_WARNNING,"event get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static int __poll_event(Event *event,void *window)
{
    dbg_str(DBG_DETAIL,"sdl event poll");
    int quit = 0;
	SDL_Event *e = &((Sdl_Event *)event)->ev;
	Component *cur;
	Window *w = (Window *)window;
	Graph *g = w->graph;
     
	//add for test>>
    Container *container;
    container = (Container *)window;
	cur = container->search_component(container,"box");
	dbg_str(DBG_WARNNING,"found cur component :%p",cur);
	//<<
	
	/*
     *String *string;
     *char buf[2048];
     *allocator_t *allocator = ((Obj *)event)->allocator;
     *string = OBJECT_NEW(allocator, String,NULL);
	 */

    SDL_StartTextInput();

    while(!quit) {
         while(SDL_PollEvent(e) != 0) {
             switch(e->type) {
                 case SDL_QUIT:
                     quit = 1; 
                     break;
                 case SDL_KEYDOWN:
                     /*
                      *print_key(&e->key.keysym, (e->key.state == SDL_PRESSED) ? SDL_TRUE : SDL_FALSE, (e->key.repeat) ? SDL_TRUE : SDL_FALSE);
                      */
                     if((e->key.repeat) ? SDL_TRUE : SDL_FALSE) {
                         break;
                     }
                     switch(e->key.keysym.sym) {
                         case SDLK_UP:
                             dbg_str(DBG_DETAIL,"SDLK_UP");
                             break;
                         case SDLK_DOWN:
                             dbg_str(DBG_DETAIL,"SDLK_DOWN");
                             break;
                         case SDLK_LEFT:
                             dbg_str(DBG_DETAIL,"SDLK_LEFT");
                             break;
                         case SDLK_RIGHT:
                             dbg_str(DBG_DETAIL,"SDLK_RIGHT");
                         case SDLK_BACKSPACE:
                             dbg_str(DBG_DETAIL,"BACKSPACE");
                             break;
                         default:
                             break;
                     } 
                     break;
                 case SDL_KEYUP:
                     /*
                      *dbg_str(DBG_DETAIL,"SDL_KEYUP");
                      */
                     break;
                 case SDL_MOUSEBUTTONDOWN:
                     dbg_str(DBG_DETAIL,"SDL_MOUSEBUTTONDOWN");
                     break;
                 case SDL_TEXTEDITING:
                     print_text("EDIT", e->text.text);
                     break;
                 case SDL_TEXTINPUT:
					 cur->text_input(cur,e->text.text[0], g);
					 /*
                      *dbg_str(DBG_DETAIL,"text:%s",e->text.text);
                      *string->append_char(string,e->text.text[0]);
					  */
                     break;
                 default:
                     break;
             }
         }
    }

	/*
	 *object_dump(string, "String", buf, 2048);
	 *dbg_str(DBG_DETAIL,"String dump: %s",buf);
	 */
    SDL_StopTextInput();

    return 0;
}

static class_info_entry_t sdl_event_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Event","event",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_FUNC_POINTER,"","poll_event",__poll_event,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Sdl_Event",sdl_event_class_info);

void test_obj_sdl_event()
{
	allocator_t *allocator = allocator_get_default_alloc();
    Window *window;
	Graph *g;
	Event *event;
    char *set_str;
    char buf[2048];

    set_str = (char *)gen_window_setting_str();

    /*
     *window  = OBJECT_NEW(allocator, Sdl_Window,set_str);
     */
    window  = OBJECT_NEW(allocator, Sdl_Window,set_str);
	g       = window->graph;
    event   = window->event;

    object_dump(window, "Sdl_Window", buf, 2048);
    dbg_str(DBG_DETAIL,"Window dump: %s",buf);

    event->poll_event(event,window);

    object_destroy(window);

    free(set_str);
}


