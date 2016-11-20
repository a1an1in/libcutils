#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/subject.h>

int subject_construct(struct subject_s *subject,char *init_str)
{
	dbg_str(DBG_SUC,"subject construct, subject addr:%p",subject);

	return 0;
}

int subject_deconstrcut(struct subject_s *subject)
{
	dbg_str(DBG_SUC,"subject deconstruct,subject addr:%p",subject);

	return 0;
}

int subject_move(struct subject_s *subject)
{
	dbg_str(DBG_DETAIL,"subject move");
}

int subject_show(struct subject_s *subject)
{
	dbg_str(DBG_DETAIL,"subject show");
}

int subject_add_x_speed(struct subject_s *subject, float v) 
{
	dbg_str(DBG_DETAIL,"subject_add_x_speed");
}

int subject_add_y_speed(struct subject_s *subject, float v) 
{
	dbg_str(DBG_DETAIL,"subject_add_x_speed");
}

int subject_is_touching(struct subject_s *me,struct subject_s *subject)
{
	dbg_str(DBG_DETAIL,"subject_is_touching");
}

int subject_set(subject_t *subject, char *attrib, void *value)
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
	} else {
		dbg_str(DBG_DETAIL,"subject set, not support %s setting",attrib);
	}

	return 0;
}

static class_info_entry_t subject_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"obj_t","obj",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_NORMAL_POINTER,"allocator_t","allocator",NULL,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",subject_set,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",subject_construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",subject_deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","move",subject_move,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","show",subject_show,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","add_x_speed",subject_add_x_speed,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","add_y_speed",subject_add_y_speed,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_VIRTUAL_FUNC_POINTER,"","is_touching",subject_is_touching,sizeof(void *)},
	[10] = {ENTRY_TYPE_INT32_T,"int","x",NULL,sizeof(int)},
	[11] = {ENTRY_TYPE_INT32_T,"int","y",NULL,sizeof(int)},
	[12] = {ENTRY_TYPE_INT32_T,"int","width",NULL,sizeof(int)},
	[13] = {ENTRY_TYPE_INT32_T,"int","height",NULL,sizeof(int)},
	[14] = {ENTRY_TYPE_FLOAT_T,"float","x_speed",NULL,sizeof(float)},
	[15] = {ENTRY_TYPE_FLOAT_T,"float","y_speed",NULL,sizeof(float)},
	[16] = {ENTRY_TYPE_END},

};

__attribute__((constructor(CONSTRUCTOR_PRIORITY_REGISTER_CLASS))) static void
register_class()
{
	object_deamon_t *deamon = object_deamon_get_global_object_deamon();

    printf("CONSTRUCTOR_PRIORITY_REGISTER_CLASS=%d,class name %s\n",
			CONSTRUCTOR_PRIORITY_REGISTER_CLASS,
			"subject_t");

	object_deamon_register_class(deamon,(char *)"subject_t", subject_class_info);
}

void test_obj_subject()
{
	subject_t *subject;
	allocator_t *allocator = allocator_get_default_alloc();
	object_deamon_t *deamon = object_deamon_get_global_object_deamon();

	dbg_str(DBG_DETAIL,"subject_subject");
	dbg_str(DBG_DETAIL,"subject_construct addr:%p",subject_construct);
	dbg_str(DBG_DETAIL,"subject_class addr:%p",subject_class_info);

	subject = OBJECT_ALLOC(allocator, subject_t);
	object_init(subject,"subject_t");

	dbg_str(DBG_DETAIL,"run at here");
	subject->move(subject);
}


