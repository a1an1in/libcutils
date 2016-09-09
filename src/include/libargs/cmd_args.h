#ifndef __CMD_ARGS_H__
#define __CMD_ARGS_H__

#include <libdata_structure/link_list.h>

typedef struct arg_entry_s{
#define MAX_ARG_NAME_LEN 50
#define MAX_HELP_INFO_LEN 100
	char cmd[MAX_ARG_NAME_LEN];
	int (*fn)(void *base, int argc, char **argv);
	char cmd_readable_name[MAX_ARG_NAME_LEN];
	char args_readable_names[MAX_ARG_NAME_LEN];
	char help_info[MAX_HELP_INFO_LEN];
	uint8_t args_count;
#undef MAX_HELP_INFO_LEN
#undef MAX_ARG_NAME_LEN
}arg_entry_t;

typedef struct args_processor_s{
	llist_t *llist;
	allocator_t *allocator;
	void *base;
}args_processor_t;

args_processor_t * args_create(allocator_t *allocator);
args_processor_t * args_init(args_processor_t *p,void *base);
int args_add_entry(args_processor_t *p,
		char *cmd,
		int (*fn)(void *base,int argc,char **argv),
		char *cmd_readable_name,
		char *args_readable_names,
		uint8_t args_count,
		char *help_info);
int args_destroy(args_processor_t *processor);
arg_entry_t * args_find_entry(args_processor_t *p,char *cmd);
int args_parse_args(args_processor_t *p,int argc, char *argv[]);
void args_print_help_info(args_processor_t *p);
#endif
