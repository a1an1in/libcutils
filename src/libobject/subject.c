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

static int __construct(Subject *subject,char *init_str)
{
	dbg_str(DBG_SUC,"subject construct, subject addr:%p",subject);

	return 0;
}

static int __deconstrcut(Subject *subject)
{
	dbg_str(DBG_SUC,"subject deconstruct,subject addr:%p",subject);

	return 0;
}

static int __move(Subject *subject)
{
	dbg_str(DBG_DETAIL,"subject move");
}

static int __show(Subject *subject)
{
	dbg_str(DBG_DETAIL,"subject show");
}

static int __add_x_speed(Subject *subject, float v) 
{
	dbg_str(DBG_DETAIL,"__add_x_speed");
}

static int __add_y_speed(Subject *subject, float v) 
{
	dbg_str(DBG_DETAIL,"__add_x_speed");
}

static int __is_touching(Subject *me,Subject *subject)
{
	dbg_str(DBG_DETAIL,"__is_touching");
}

static int __set(Subject *subject, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		subject->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		subject->get = value;
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

static void * __get(Subject *obj, char *attrib)
{
    if(strcmp(attrib, "x") == 0) 
        return &obj->x;
    else if(strcmp(attrib, "y") == 0) 
        return &obj->y;
    else if(strcmp(attrib, "width") == 0) 
        return &obj->width;
    else if(strcmp(attrib, "height") == 0) 
        return &obj->height;
    else if(strcmp(attrib, "x_speed") == 0) 
        return &obj->x_speed;
    else if(strcmp(attrib, "y_speed") == 0) {
        return &obj->y_speed;
    } else {
        dbg_str(DBG_WARNNING,"subject get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t subject_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","move",__move,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","show",__show,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","add_x_speed",__add_x_speed,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","add_y_speed",__add_y_speed,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","is_touching",__is_touching,sizeof(void *)},
	[10] = {ENTRY_TYPE_INT32_T,"int","x",NULL,sizeof(int)},
	[11] = {ENTRY_TYPE_INT32_T,"int","y",NULL,sizeof(int)},
	[12] = {ENTRY_TYPE_INT32_T,"int","width",NULL,sizeof(int)},
	[13] = {ENTRY_TYPE_INT32_T,"int","height",NULL,sizeof(int)},
	[14] = {ENTRY_TYPE_FLOAT_T,"float","x_speed",NULL,sizeof(float)},
	[15] = {ENTRY_TYPE_FLOAT_T,"float","y_speed",NULL,sizeof(float)},
	[16] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Subject",subject_class_info);

void test_obj_subject()
{
	Subject *subject;
	allocator_t *allocator = allocator_get_default_alloc();

    subject = OBJECT_NEW(allocator, Subject,"");

    subject->move(subject);
}


