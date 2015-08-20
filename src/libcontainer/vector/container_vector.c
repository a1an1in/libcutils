/*
 * =====================================================================================
 *
 *       Filename:  container_vector.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/12/2015 09:11:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
/*  
 * Copyright (c) 2015-2010 alan lin <a1an1in@sina.com>
 *  
 *  
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
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "libcontainer/container_list.h"
#include "libcontainer/inc_files.h"

iterator_t vector_iterator_init(iterator_t *it,uint32_t pos,container_t *ct);
int vector_iterator_equal(iterator_t it1,iterator_t it2);

int vector_init(container_t *ct)
{
	dbg_str(DBG_CONTAINER_DETAIL,"vector init");
	vector_priv_t *vp_p = &ct->priv.vector_priv;
	uint32_t data_size = vp_p->data_size;
	uint32_t capacity  = vp_p->capacity;

	ct->container_type = CONTAINER_TYPE_VECTOR;
	vp_p->step = data_size;
	strcpy(ct->name,"vector container");
	vp_p->vector_head = allocator_mem_alloc(
			ct->allocator, capacity * (ct->priv.vector_priv.step));
	if(vp_p->vector_head == NULL){
		dbg_str(DBG_CONTAINER_ERROR,"vector_init");
	}
	dbg_str(DBG_CONTAINER_DETAIL,"vector_head:%p,size=%d",
			ct->priv.vector_priv.vector_head,
			capacity * (ct->priv.vector_priv.step)
			);
	vector_iterator_init(&ct->begin,0,ct);
	vector_iterator_init(&ct->end,0,ct);
	return 0;
}
int vector_push_front(container_t *ct,void *data)
{
	dbg_str(DBG_CONTAINER_WARNNING,"not support vector_push_front");
	return 0;
}
int vector_push_back(container_t *ct,void *data)
{
	vector_priv_t *vp_p = &ct->priv.vector_priv;
	uint32_t data_size = vp_p->data_size;
	void *vector_head  = vp_p->vector_head;
	uint32_t step      = vp_p->step;
	uint32_t capacity  = vp_p->capacity;
	uint32_t push_pos  = ct->end.pos.vector_pos;

	pthread_rwlock_wrlock(&ct->head_lock);
	dbg_str(DBG_CONTAINER_DETAIL,"vector_push_back,push_pos=%d,capacity=%d",push_pos,capacity);
	if(push_pos < capacity){
		memcpy(vector_head + (push_pos++)*step,data,data_size);
		vector_iterator_init(&ct->end,push_pos,ct);
	}else{
		dbg_str(DBG_CONTAINER_WARNNING,"realloc mem");
		vp_p->vector_head = allocator_mem_alloc(
				ct->allocator, 2*capacity * (ct->priv.vector_priv.step));
		if(vp_p->vector_head == NULL){
			dbg_str(DBG_CONTAINER_ERROR,"vector_push_back,realloc mem");
		}
		vp_p->capacity = 2*capacity;
		memcpy(vp_p->vector_head,vector_head,capacity*step);
		memcpy(vp_p->vector_head + (push_pos++)*step,data,data_size);
		vector_iterator_init(&ct->end,push_pos,ct);
		allocator_mem_free(ct->allocator,vector_head);
	}
	pthread_rwlock_unlock(&ct->head_lock);

	return 0;
}

int vector_pop_front(container_t *ct)
{
	dbg_str(DBG_CONTAINER_WARNNING,"not support vector_pop_front");
	return 0;
}
int vector_pop_back(container_t *ct)
{
	uint32_t pop_pos  = ct->end.pos.vector_pos;
	
	dbg_str(DBG_CONTAINER_DETAIL,"pop back");

	pthread_rwlock_wrlock(&ct->head_lock);
	if(!vector_iterator_equal(ct->begin,ct->end)){
		vector_iterator_init(&ct->end,--pop_pos,ct);
	} else{
		dbg_str(DBG_CONTAINER_WARNNING,"vector is NULL");
	}
	pthread_rwlock_unlock(&ct->head_lock);

	return 0;
}
int vector_copy_backward(container_t *ct,iterator_t to,iterator_t from,int32_t count)
{
	uint32_t from_pos, to_pos;
	vector_priv_t *vp_p = &ct->priv.vector_priv;
	void *vector_head   = vp_p->vector_head;
	uint32_t step       = vp_p->step;
	uint32_t num_per;
	
	from_pos   = from.pos.vector_pos + count;
	to_pos     = to.pos.vector_pos + count;

	num_per = to_pos - from_pos;
	for(;count > 0;){
		to_pos   -= num_per;
		from_pos -= num_per;
		count    -= num_per;
		num_per   = count - num_per > 0 ? num_per :count;
		dbg_str(DBG_CONTAINER_DETAIL,"to_pos=%d,from_pos=%d",to_pos,from_pos);
		memcpy(vector_head + to_pos * step, vector_head + from_pos * step, num_per * step);
	}
}
int vector_copy_forward(container_t *ct,iterator_t to,iterator_t from,int32_t count)
{
	uint32_t from_pos = from.pos.vector_pos;
	uint32_t to_pos = to.pos.vector_pos;
	vector_priv_t *vp_p = &ct->priv.vector_priv;
	void *vector_head  = vp_p->vector_head;
	uint32_t step      = vp_p->step;
	uint32_t num_per;
	
	num_per = from_pos - to_pos;
	for(;count > 0;){
		num_per = count - num_per > 0 ? num_per :count;
		dbg_str(DBG_CONTAINER_DETAIL,"to_pos=%d,from_pos=%d,num_per=%d",to_pos,from_pos,num_per);
		memcpy(vector_head + to_pos * step,vector_head + from_pos * step,num_per * step);
		to_pos += num_per;
		from_pos += num_per;
		count -= num_per;
	}
}
int vector_copy(container_t *ct,iterator_t to,iterator_t from,int32_t count)
{
	uint32_t from_pos = from.pos.vector_pos;
	uint32_t to_pos = to.pos.vector_pos;
	
	dbg_str(DBG_CONTAINER_WARNNING,"count=%d",count);
	if(from_pos > to_pos){//forward
		vector_copy_forward(ct,to,from,count);
	}else{//backward
		vector_copy_backward(ct,to,from,count);
	}
}
int vector_insert(container_t *ct, iterator_t it, void *data)
{

	uint32_t insert_pos  = it.pos.vector_pos;
	uint32_t end_pos = ct->end.pos.vector_pos;
	uint32_t count = end_pos - insert_pos;
	vector_priv_t *vp_p = &ct->priv.vector_priv;
	void *vector_head  = vp_p->vector_head;
	uint32_t step      = vp_p->step;
	iterator_t to;
	
	dbg_str(DBG_CONTAINER_DETAIL,"insert vector");
	vector_iterator_init(&to,insert_pos + 1,ct);
	dbg_str(DBG_CONTAINER_DETAIL,"insert_pos=%d,to_pos=%d",insert_pos,to.pos.vector_pos);

	pthread_rwlock_wrlock(&ct->head_lock);
	vector_copy(ct,to,it,count);
	memcpy(vector_head + insert_pos * step,data,step);
	vector_iterator_init(&ct->end,end_pos + 1,ct);
	pthread_rwlock_unlock(&ct->head_lock);

	return 0;
}
int vector_delete(container_t *ct, iterator_t it)
{
	uint32_t delete_pos  = it.pos.vector_pos;
	uint32_t end_pos = ct->end.pos.vector_pos;
	uint32_t count = end_pos - delete_pos;
	iterator_t from;

	vector_iterator_init(&from,delete_pos + 1,ct);

	pthread_rwlock_wrlock(&ct->head_lock);
	if(vector_iterator_equal(it,ct->end)){
		dbg_str(DBG_CONTAINER_WARNNING,"can't del end iterator");
	}else if(vector_iterator_equal(it,ct->begin)&&vector_iterator_equal(from,ct->end)){
		dbg_str(DBG_CONTAINER_WARNNING,"vector is NULL");
	}else{
		vector_copy(ct,it,from,count);
		vector_iterator_init(&ct->end,end_pos - 1,ct);
	}
	pthread_rwlock_unlock(&ct->head_lock);

	return 0;
}
int vector_destroy(container_t *ct)
{
	dbg_str(DBG_CONTAINER_DETAIL,"vector_destroy");
	allocator_mem_free(ct->allocator,ct->priv.vector_priv.vector_head);
}

iterator_t vector_begin(container_t *ct)
{
	return ct->begin;
}

iterator_t vector_end(container_t *ct)
{
	return ct->end;
}

iterator_t vector_iterator_init(iterator_t *it,uint32_t pos,container_t *ct)
{
	it->container_p    = ct;
	it->pos.vector_pos = pos;

	return *it;
}
iterator_t vector_iterator_next(iterator_t it)
{
	return vector_iterator_init(&it,++it.pos.vector_pos,it.container_p);
}
iterator_t vector_iterator_prev(iterator_t it)
{
	return vector_iterator_init(&it,--it.pos.vector_pos,it.container_p);
}
int vector_iterator_equal(iterator_t it1,iterator_t it2)
{
	return it1.pos.vector_pos == it2.pos.vector_pos;
}
void *vector_iterator_get_pointer(iterator_t it)
{
	vector_priv_t *vp_p = &it.container_p->priv.vector_priv;
	return vp_p->vector_head + it.pos.vector_pos * (vp_p->step);
}

int container_vector_register(){
	container_module_t cm = {
		.name = "container_vector",
		.container_type = CONTAINER_TYPE_VECTOR,
		.c_ops = {
			.init       = vector_init,
			.push_front = vector_push_front,
			.push_back  = vector_push_back,
			.pop_front  = vector_pop_front,
			.pop_back   = vector_pop_back,
			.begin      = vector_begin,
			.end        = vector_end,
			.del        = vector_delete,
			.insert     = vector_insert,
			.destroy 	= vector_destroy,
		},
		.it_ops = {
			vector_iterator_next,
			vector_iterator_prev,
			vector_iterator_equal,
			vector_iterator_get_pointer
		}
	};
	memcpy(&container_modules[CONTAINER_TYPE_VECTOR],&cm,sizeof(container_module_t));
	return 0;
}

