/*
 * =====================================================================================
 *
 *       Filename:  link_list.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/14/2015 09:30:34 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __LINK_LIST_H__
#define __LINK_LIST_H__

#include "link_list_struct.h"

llist_t *llist_create(allocator_t *allocator,uint8_t lock_type);
int llist_init(llist_t *llist,uint32_t data_size);
int llist_insert(llist_t *llist, list_pos_t pos, void *data);
int llist_delete(llist_t *llist, list_pos_t pos);
list_t *llist_detach(llist_t *llist, list_pos_t pos);
int llist_push_back(llist_t *llist,void *data);
int llist_pop_back(llist_t *llist);
int llist_destroy(llist_t *llist);
void *llist_pos_get_pointer(list_pos_t pos);
void llist_for_each(llist_t *llist,void (*func)(list_t *list));

static inline int llist_pos_init(list_pos_t *pos,struct list_head *lh,llist_t *llist)
{

	pos->llist = llist;
	pos->list_head_p = lh;
	return 0;
}
static inline list_pos_t llist_begin(llist_t *llist)
{
	return llist->begin;
}

static inline list_pos_t llist_end(llist_t *llist)
{
	return llist->head;
}
static inline list_pos_t llist_pos_next(list_pos_t pos)
{
	list_pos_t ret;

	llist_pos_init(&ret,pos.list_head_p->next,pos.llist);

	return ret;
}
static inline list_pos_t llist_pos_prev(list_pos_t pos)
{
	list_pos_t ret;

	llist_pos_init(&ret,pos.list_head_p->prev,pos.llist);

	return ret;
}
static inline int llist_pos_equal(list_pos_t pos1,list_pos_t pos2)
{
	return pos1.list_head_p == pos2.list_head_p;
}
static inline int llist_push_front(llist_t *llist,void *data)
{
	return llist_insert(llist,llist->head,data);
}
static inline int llist_pop_front(llist_t *llist)
{
	return llist_delete(llist, llist->begin);
}
static inline list_t *llist_detach_front(llist_t *llist)
{
	return llist_detach(llist, llist->begin);
}
#endif
