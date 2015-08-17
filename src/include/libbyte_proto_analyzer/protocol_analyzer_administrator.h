/*
 * =====================================================================================
 *
 *       Filename:  protocol_analyzer_administrator.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/11/2015 01:04:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __PROTOCOL_ANALYZER_ADMINISTRATOR_H__
#define __PROTOCOL_ANALYZER_ADMINISTRATOR_H__
#include "protocol_analyzer.h"
#include "protocol_format_set.h"

#define LIST(type) \
	struct{\
		type data;\
		struct list_head list_head;\
	}

struct free_pa_hlist_data{
	int count;
};
struct free_pa_list_data{
	struct protocol_analyzer_s *pa;
};
typedef struct proto_analyzer_admin_s{
	/*
	 *pthread_rwlock_t head_lock;
	 */
	protocol_format_set_t *pfs;
	struct list_head *free_hl_head;
}proto_analyzer_admin_t;

void proto_analyzer_admin_init(struct proto_analyzer_admin_s **pa_admin, protocol_format_set_t *pfs);
struct protocol_analyzer_s * proto_analyer_admin_get_pa(struct proto_analyzer_admin_s *pa_admin,uint32_t proto_id);

#endif
