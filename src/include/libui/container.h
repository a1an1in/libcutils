#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include <stdio.h>
#include <libdbg/debug.h>
#include <libui/subject.h>

typedef struct container_s Container;

struct container_s{
	Subject subject;

	int (*construct)(Container *container,char *init_str);
	int (*deconstruct)(Container *container);
	int (*set)(Container *container, char *attrib, void *value);
    void *(*get)(void *obj, char *attrib);

	/*virtual methods reimplement*/
	int (*move)(Container *container);
#define MAX_NAME_LEN 50
    char name[MAX_NAME_LEN];
#undef MAX_NAME_LEN

};

#endif
