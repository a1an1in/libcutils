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

int llist_pos_init(list_pos_t *pos,struct list_head *lh,llist_t *llist);
list_pos_t llist_begin(llist_t *llist);
list_pos_t llist_end(llist_t *llist);
list_pos_t llist_pos_next(list_pos_t pos);
list_pos_t llist_pos_prev(list_pos_t pos);
int llist_pos_equal(list_pos_t pos1,list_pos_t pos2);
llist_t *llist_create();
int llist_init(llist_t *llist,uint32_t data_size);
int llist_insert(llist_t *llist, list_pos_t pos, void *data);
int llist_delete(llist_t *llist, list_pos_t pos);
int llist_push_front(llist_t *llist,void *data);
int llist_push_back(llist_t *llist,void *data);
int llist_pop_back(llist_t *llist);
int llist_pop_front(llist_t *llist);
int llist_destroy(llist_t *llist);
void *llist_pos_get_pointer(list_pos_t pos);
void llist_for_each(llist_t *llist,void (*func)(list_t *list));
#endif
