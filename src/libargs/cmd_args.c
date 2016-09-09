/**
 * @file cmd_args.c
 * @synopsis 
 * @author a1an1in@sina.com
 * @version 1.0
 * @date 2016-09-08
 */
/* Copyright (c) 2015-2020 alan lin <a1an1in@sina.com>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <libdbg/debug.h>
#include <libargs/cmd_args.h>

args_processor_t * 
args_create(allocator_t *allocator)
{
	args_processor_t *processor;

	processor = (args_processor_t*)allocator_mem_alloc(allocator,sizeof(args_processor_t));
	if(processor == NULL){
		dbg_str(ARGS_ERROR,"allocator_mem_alloc");
		return NULL;
	}
	processor->llist = llist_create(allocator,0);
	if(processor->llist == NULL){
		dbg_str(ARGS_ERROR,"llist_create");
		allocator_mem_free(allocator,processor);
		return NULL;
	}
	llist_init(processor->llist,sizeof(arg_entry_t));
	processor->allocator = allocator;

	return processor;
}
args_processor_t * 
args_init(args_processor_t *p,void *base)
{
	p->base = base;

	return p;
}
static inline int 
__args_add_entry(args_processor_t *p,arg_entry_t *e)
{
	return llist_push_back(p->llist,e);
}
int args_add_entry(args_processor_t *p,
		char *cmd,
		int (*fn)(void *base,int argc,char **argv),
		char *cmd_readable_name,
		char *args_readable_names,
		uint8_t args_count,
		char *help_info)
{
	arg_entry_t e;

	memset(&e,0, sizeof(arg_entry_t));

	strncpy(e.cmd,cmd,strlen(cmd));
	e.fn         = fn;
	e.args_count = args_count;
	strncpy(e.cmd_readable_name,cmd_readable_name,strlen(cmd_readable_name));
	strncpy(e.args_readable_names,args_readable_names,strlen(args_readable_names));
	strncpy(e.help_info,help_info,strlen(help_info));

	return __args_add_entry(p,&e);
}
arg_entry_t * 
args_find_entry(args_processor_t *p,char *cmd)
{
	llist_t *llist = p->llist;
	list_t *list;
	list_pos_t pos,next;

	for(	llist_begin(llist, &pos), llist_pos_next(&pos,&next);
			!llist_pos_equal(&pos,&llist->head);
			pos = next, llist_pos_next(&pos,&next))
	{
		list = container_of(pos.list_head_p,list_t,list_head);
		arg_entry_t *e = (arg_entry_t *)list->data;
		if(!strcmp(cmd,e->cmd)){
			return e;
		}
	}

	dbg_str(ARGS_DETAIL,"not find cmd");

	return NULL;
}
int 
args_parse_args(args_processor_t *p,int argc, char *argv[])
{
	int i = 0;
	arg_entry_t *e; 
	uint8_t args_count;

	for(i = 0; i < argc;){
		e = args_find_entry(p,argv[i]);
		if(e == NULL){
			dbg_str(ARGS_WARNNING,"%s is not valid cmd,please check.We'll skip this cmd",argv[i]);
			i++;
			continue;
		} else {
			dbg_str(ARGS_DETAIL,"process cmd %s",argv[i]);
			i++;
			args_count = e->fn(p->base,argc - i,argv + i);
			if(args_count != e->args_count){
				dbg_str(ARGS_WARNNING,"the args funtion process args count is not equal the setting,"
						"we'll compute by the default setting");
			}
			i+= e->args_count;
		}
	}

	return 0;
}
void args_print_help_info(args_processor_t *p)
{
	llist_t *llist = p->llist;
	list_t *list;
	list_pos_t pos,next;
	arg_entry_t *e;
	char r1[]="cmd name";
	char r2[]="format";
	char r3[]="description";

	for(	llist_begin(llist, &pos), llist_pos_next(&pos,&next);
			!llist_pos_equal(&pos,&llist->head);
			pos = next, llist_pos_next(&pos,&next))
	{
		list = container_of(pos.list_head_p,list_t,list_head);
		e = (arg_entry_t *)list->data;
		printf("%15s:%s\n",r1,e->cmd_readable_name);
		printf("%15s:%s %s\n",r2,e->cmd,e->args_readable_names);
		printf("%15s:%s\n\n",r3,e->help_info);
	}

	return;
}
int args_destroy(args_processor_t *processor)
{
	dbg_str(ARGS_DETAIL,"args_destroy");

	llist_destroy(processor->llist);
	allocator_mem_free(processor->allocator,processor);
	return 0;
}
