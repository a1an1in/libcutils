#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/subject.h>

typedef struct enemy_s Enemy;

struct enemy_s{
	Subject subject;

	int (*construct)(Enemy *enemy,char *init_str);
	int (*deconstruct)(Enemy *enemy);
	int (*set)(Enemy *enemy, char *attrib, void *value);

	/*virtual methods reimplement*/
	int (*move)(Enemy *enemy);

};

static int enemy_construct(Enemy *enemy,char *init_str)
{
	dbg_str(DBG_SUC,"enemy construct, enemy addr:%p",enemy);

	return 0;
}

static int enemy_deconstrcut(Enemy *enemy)
{
	dbg_str(DBG_SUC,"enemy deconstruct,enemy addr:%p",enemy);

	return 0;
}

static int enemy_move(Enemy *enemy)
{
	dbg_str(DBG_SUC,"enemy move");
}

static int enemy_set(Enemy *enemy, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		enemy->set = value;
	} else if(strcmp(attrib, "construct") == 0) {
		enemy->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		enemy->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
		enemy->move = value;
	} else {
		dbg_str(DBG_DETAIL,"enemy set, not support %s setting",attrib);
	}

	return 0;
}

static class_info_entry_t enemy_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Subject","subject",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_NORMAL_POINTER,"allocator_t","allocator",NULL,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","set",enemy_set,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",enemy_construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",enemy_deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_FUNC_POINTER,"","move",enemy_move,sizeof(void *)},
	[6] = {ENTRY_TYPE_END},

};

__attribute__((constructor(CONSTRUCTOR_PRIORITY_REGISTER_CLASS))) static void
register_class()
{
	object_deamon_t *deamon = object_deamon_get_global_object_deamon();

    printf("CONSTRUCTOR_PRIORITY_REGISTER_CLASS=%d,class name %s\n",
			CONSTRUCTOR_PRIORITY_REGISTER_CLASS,
			"Enemy");

	object_deamon_register_class(deamon,(char *)"Enemy", enemy_class_info);
}

void test_obj_enemy()
{
    Subject *subject;

	allocator_t *allocator = allocator_get_default_alloc();

    subject = OBJECT_NEW(allocator, Enemy,"");

	subject->move(subject);
}


