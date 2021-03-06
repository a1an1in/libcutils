#ifndef __VECTOR_H__
#define __VECTOR_H__
#include <libdata_structure/vector_struct.h>
static inline vector_pos_t * vector_pos_init(vector_pos_t *it,uint32_t pos,vector_t *vt)
{
	it->vector      = vt;
	it->vector_pos  = pos;
	it->vector_head = vt->vector_head;
	return it;
}
static inline vector_pos_t * vector_begin(vector_t *vt,vector_pos_t *it)
{
	return vector_pos_init(it,0,vt);
}
static inline vector_pos_t * vector_end(vector_t *vt,vector_pos_t *it)
{
	return vector_pos_init(it,vt->end.vector_pos,vt);
}
static inline vector_pos_t * vector_pos_next(vector_pos_t *it,vector_pos_t *next)
{
	return vector_pos_init(next,it->vector_pos + 1,it->vector);
}
static inline vector_pos_t * vector_pos_prev(vector_pos_t *it,vector_pos_t *prev)
{
	return vector_pos_init(prev,(it->vector_pos > 1) ? (it->vector_pos - 1) : 0,it->vector);
}
static inline int vector_pos_equal(vector_pos_t *it1,vector_pos_t *it2)
{
	return it1->vector_pos == it2->vector_pos;
}
static inline void *vector_pos_get_pointer(vector_pos_t *it)
{
	vector_t *vector = it->vector;
	return vector->vector_head + it->vector_pos * (vector->step);
}
static inline void vector_for_each(vector_t *vector,void (*func)(void *))
{
	vector_pos_t pos,next;

	for(	vector_begin(vector, &pos), vector_pos_next(&pos,&next);
			!vector_pos_equal(&pos,&vector->end);
			pos = next, vector_pos_next(&pos,&next))
	{
		func(&pos);
	}
}

static inline void vector_for_each_with_arg(vector_t *vector,void (*func)(void *,void *arg),void *arg)
{
	vector_pos_t pos,next;

	for(	vector_begin(vector, &pos), vector_pos_next(&pos,&next);
			!vector_pos_equal(&pos,&vector->end);
			pos = next, vector_pos_next(&pos,&next))
	{
        func(&pos,arg);
	}
}
int vector_copy_backward(vector_t *vector,vector_pos_t *to,vector_pos_t *from,uint32_t count);
int vector_copy_forward(vector_t *vector,vector_pos_t *to,vector_pos_t *from,uint32_t count);
int vector_copy(vector_t *vector,vector_pos_t *to,vector_pos_t *from,uint32_t count);
vector_t *vector_create(allocator_t *allocator,uint8_t lock_type);
int vector_init(vector_t *vector,uint32_t data_size,uint32_t capacity);
int vector_push_front(vector_t *vector,void *data);
int vector_push_back(vector_t *vector,void *data);
int vector_pop_front(vector_t *vector);
int vector_pop_back(vector_t *vector);
int vector_insert(vector_t *vector, vector_pos_t *it, void *data);
int vector_delete(vector_t *vector, vector_pos_t *it);
int vector_set(vector_t *vector,int index,void *data);
void * vector_get(vector_t *vector,int index);
int vector_destroy(vector_t *vt);


#endif
