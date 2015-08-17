/*
 * =====================================================================================
 *
 *       Filename:  protocol_analyzer_administrator.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/11/2015 12:58:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "libdbg/debug.h"
#include "libbyte_proto_analyzer/protocol_analyzer.h"
#include "libbyte_proto_analyzer/protocol_analyzer_administrator.h"

/*
 *#define LIST(type) \
 *    struct{\
 *        type data;\
 *        struct list_head list_head;\
 *    }
 *
 *struct free_pa_hlist_data{
 *    int count;
 *};
 *struct free_pa_list_data{
 *    struct protocol_analyzer_s *pa;
 *};
 *typedef struct proto_analyzer_admin_s{
 *    protocol_format_set_t *pfs;
 *    struct list_head *free_hl_head;
 *}proto_analyzer_admin_t;
 */
void proto_analyzer_admin_init(struct proto_analyzer_admin_s **pa_admin, protocol_format_set_t *pfs)
{
	LIST(struct free_pa_hlist_data) *head_list;
	 
	if(*pa_admin == NULL){
		*pa_admin = mem_alloc_pck(NULL,sizeof(struct proto_analyzer_admin_s),0);
		if(*pa_admin == NULL){
			dbg_str(DBG_ERROR,"create pa_admin err");
			return;
		}
	}
	head_list = mem_alloc_pck(NULL,sizeof(*head_list),0);
	if(head_list == NULL){
		dbg_str(DBG_ERROR,"create head_list err");
		return;
	}
	/*
	 *pthread_rwlock_init(&head_list->head_lock,NULL);
	 */
	dbg_str(DBG_DETAIL,"run at here");
	head_list->data.count = 0;
	INIT_LIST_HEAD(&head_list->list_head);
	(*pa_admin)->free_hl_head = &head_list->list_head;
	(*pa_admin)->pfs = pfs;
	dbg_str(DBG_DETAIL,"run at here");
}
struct protocol_analyzer_s * proto_analyer_admin_get_pa(struct proto_analyzer_admin_s *pa_admin,uint32_t proto_id)
{
	struct protocol_analyzer_s *pa = NULL;
	LIST(struct free_pa_hlist_data) *head_list;

	if(pa_admin->free_hl_head){
		head_list = (LIST(struct free_pa_hlist_data) *)container_of(pa_admin->free_hl_head,LIST(struct free_pa_hlist_data),list_head);
		if(head_list->data.count == 0){
			pa_init_protocol_analyzer(proto_id, pa_admin, &pa);
		}else{
			dbg_str(DBG_DETAIL,"have reclaim pa");
		}
	}else{
		pa_init_protocol_analyzer(proto_id, pa_admin, &pa);
	}

	return pa;
}
void proto_analyer_admin_reclaim_pa(struct protocol_analyzer_s *pa)
{

}
