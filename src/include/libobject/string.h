#ifndef __STRING_H__
#define __STRING_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/obj.h>

typedef struct string_s String;

struct string_s{
	Obj obj;

	int (*construct)(String *string,char *init_str);
	int (*deconstruct)(String *string);
	int (*set)(String *string, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);
    String *(*assign)(String *string,char *s);

	/*virtual methods reimplement*/
#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN
    char *value;
    int value_max_len;
    int value_len;
};

#endif