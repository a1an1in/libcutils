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
#endif
