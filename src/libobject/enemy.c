#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/subject.h>

typedef struct enemy_s{
	struct subject_s subject;

	int (*construct)(struct enemy_s *enemy,char *init_str);
	int (*deconstruct)(struct enemy_s *enemy);
	int (*set)(struct enemy_s *enemy, char *attrib, void *value);

	/*virtual methods reimplement*/
	int (*move)(struct enemy_s*enemy);

}enemy_t;

int enemy_construct(struct enemy_s *enemy,char *init_str)
{
	dbg_str(DBG_SUC,"enemy construct, enemy addr:%p",enemy);

	return 0;
}

int enemy_deconstrcut(struct enemy_s *enemy)
{
	dbg_str(DBG_SUC,"enemy deconstruct,enemy addr:%p",enemy);

	return 0;
}

int enemy_move(struct enemy_s*enemy)
{
	dbg_str(DBG_SUC,"enemy move");
}

int enemy_set(enemy_t *enemy, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		enemy->set = value;
	} else if(strcmp(attrib, "construct") == 0) {
		enemy->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		enemy->deconstruct = value;
	} else {
		dbg_str(DBG_DETAIL,"enemy set, not support %s setting",attrib);
	}

	return 0;
}

static class_info_entry_t enemy_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"subject_t","subject",NULL,sizeof(void *)},
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
			"enemy_t");

	object_deamon_register_class(deamon,(char *)"enemy_t", enemy_class_info);
}

void test_obj_enemy()
{
	enemy_t *enemy;
	allocator_t *allocator = allocator_get_default_alloc();
	object_deamon_t *deamon = object_deamon_get_global_object_deamon();

	dbg_str(DBG_DETAIL,"enemy_enemy");
	dbg_str(DBG_DETAIL,"enemy_construct addr:%p",enemy_construct);
	dbg_str(DBG_DETAIL,"enemy_class addr:%p",enemy_class_info);

	enemy = OBJECT_ALLOC(allocator, enemy_t);
	object_init(enemy,"enemy_t");

	enemy->subject.move((subject_t *)enemy);
}


