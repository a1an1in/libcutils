/**
 * @file subject.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/subject.h>

static int subject_construct(Subject *subject,char *init_str)
{
	dbg_str(DBG_SUC,"subject construct, subject addr:%p",subject);

	return 0;
}

static int subject_deconstrcut(Subject *subject)
{
	dbg_str(DBG_SUC,"subject deconstruct,subject addr:%p",subject);

	return 0;
}

static int subject_move(Subject *subject)
{
	dbg_str(DBG_DETAIL,"subject move");
}

static int subject_show(Subject *subject)
{
	dbg_str(DBG_DETAIL,"subject show");
}

static int subject_add_x_speed(Subject *subject, float v) 
{
	dbg_str(DBG_DETAIL,"subject_add_x_speed");
}

static int subject_add_y_speed(Subject *subject, float v) 
{
	dbg_str(DBG_DETAIL,"subject_add_x_speed");
}

static int subject_is_touching(Subject *me,Subject *subject)
{
	dbg_str(DBG_DETAIL,"subject_is_touching");
}

static int subject_set(Subject *subject, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		subject->set = value;
	} else if(strcmp(attrib, "construct") == 0) {
		subject->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		subject->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
		subject->move = value;
	} else if(strcmp(attrib, "show") == 0) {
		subject->show = value;
	} else if(strcmp(attrib, "add_x_speed") == 0) {
		subject->add_x_speed = value;
	} else if(strcmp(attrib, "add_y_speed") == 0) {
		subject->add_y_speed = value;
	} else if(strcmp(attrib, "is_touching") == 0) {
		subject->is_touching = value;
	} else if(strcmp(attrib, "x") == 0) {
        subject->x = *((uint32_t *)value);
	} else if(strcmp(attrib, "y") == 0) {
        subject->y = *((uint32_t *)value);
	} else if(strcmp(attrib, "width") == 0) {
        subject->width = *((uint32_t *)value);
	} else if(strcmp(attrib, "height") == 0) {
        subject->height = *((uint32_t *)value);
	} else {
		dbg_str(DBG_WARNNING,"subject set,  \"%s\" setting is not support",attrib);
	}

	return 0;
}

static class_info_entry_t subject_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",subject_set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",subject_construct,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",subject_deconstrcut,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","move",subject_move,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","show",subject_show,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","add_x_speed",subject_add_x_speed,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","add_y_speed",subject_add_y_speed,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","is_touching",subject_is_touching,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_INT32_T,"int","x",NULL,sizeof(int)},
	[10] = {ENTRY_TYPE_INT32_T,"int","y",NULL,sizeof(int)},
	[11] = {ENTRY_TYPE_INT32_T,"int","width",NULL,sizeof(int)},
	[12] = {ENTRY_TYPE_INT32_T,"int","height",NULL,sizeof(int)},
	[13] = {ENTRY_TYPE_FLOAT_T,"float","x_speed",NULL,sizeof(float)},
	[14] = {ENTRY_TYPE_FLOAT_T,"float","y_speed",NULL,sizeof(float)},
	[15] = {ENTRY_TYPE_END},

};

__attribute__((constructor(CONSTRUCTOR_PRIORITY_REGISTER_CLASS))) static void
register_class()
{
	object_deamon_t *deamon = object_deamon_get_global_object_deamon();

    printf("CONSTRUCTOR_PRIORITY_REGISTER_CLASS=%d,class name %s\n",
			CONSTRUCTOR_PRIORITY_REGISTER_CLASS,
			"Subject");

	object_deamon_register_class(deamon,(char *)"Subject", subject_class_info);
}

void test_obj_subject()
{
	Subject *subject;
	allocator_t *allocator = allocator_get_default_alloc();

    subject = OBJECT_NEW(allocator, Subject,"");

    subject->move(subject);
}


