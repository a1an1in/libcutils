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

#include <libdata_structure/link_list.h>
#include <libdbg/debug.h>

typedef struct arg_entry_s{
}arg_entry_t;

typedef struct args_processor_s{
	llist_t *llist;
	allocator_t *allocator;
}args_processor_t;

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

	/*
	 *llist_push_back(llist,&t1);
	 *llist_push_back(llist,&t2);
	 *llist_for_each(llist,print_list_data);
	 */
	return processor;
}
int args_add_entry(args_processor_t *p,arg_entry_t *e)
{
	return llist_push_back(p->llist,e);
	/*
	 *llist_push_back(llist,&t2);
	 *llist_for_each(llist,print_list_data);
	 */
}
int args_destroy(args_processor_t *processor)
{
	llist_destroy(processor->llist);
	allocator_mem_free(processor->allocator,processor);
	return 0;
}
