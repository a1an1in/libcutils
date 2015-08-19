/*
 * =====================================================================================
 *
 *       Filename:  test_pdu_proto_analyzer.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/18/2015 07:24:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <ctype.h>
#include <pthread.h>
#include <math.h>
#include "libdbg/debug.h"
#include "liballoc/allocator.h"
#include "libproto_analyzer/protocol_format_set.h"
#include "libproto_analyzer/protocol_analyzer.h"
#include "libproto_analyzer/pdt_proto_format.h"
#include "libproto_analyzer/pdt_init_proto.h"

allocator_t *allocator;

int test_pdu_proto_analyzer()
{
	protocol_format_set_t *pfs_p;
	struct protocol_analyzer_s *pa;
	uint32_t proto_no = 0x3008;
	uint32_t ret = 0;
	proto_info_list_t *info_list = NULL; 
	uint8_t data_buf[8] = {1,2,3,4,5,6,7,8};
	uint8_t slc_data[4] = {0x11,0x22,0x33,0x44};
	uint8_t proto_data[16] = {
		0x30,0x8,0x18,0x34,0x56,0x78,0x0,0x0,0x2,0x10,
		0x0,0x5,0x5,0x5,0x5,0x5};
	uint8_t proto_data2[23] ={
		0x0,0x58,0x1,0x33,0x6,0x7,0x0,0x0,0x2,0x10,
		0x0,0x5,0x5,0x5,64,0x1,0x2,0x3,0x4,0x5,
		0x6,0x7,0x8
	};

	struct proto_analyzer_admin_s *pa_admin = NULL;


	allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC);

	pfs_p = pfs_create_proto_format_set(allocator);
	init_proto_format_set(0x0,100,pfs_p);

	/*
	 *pfs_print_proto_link_lists(pfs_p);
	 */

	/*set and parse test*/
	pa = pa_create_protocol_analyzer(allocator);
	pa_init_protocol_analyzer(0x8, pfs_p, pa);
	/*
	 *pa = proto_analyer_admin_get_pa(pa_admin,0x3008);
	 */

	pa_set_value("LB",1,pa);
	pa_set_value("PF",1,pa);
	pa_set_value("CSBKO",0x2f,pa);
	pa_set_value("FID",0x8,pa);
	pa_set_value("RI",1,pa);
	pa_set_value("ARC",4,pa);
	pa_set_value("RSVD",1,pa);
	pa_set_value("TADDR",0xfffc001,pa);
	pa_set_value("SADDR",0xfffc002,pa);

	dbg_str(DBG_IMPORTANT,"print list for each,orinal data");
	pfs_print_list_for_each(pa->pa_list_head_p);

	pa_generate_protocol_data(pa);

	dbg_str(DBG_DETAIL,"pa_parse_protocol_data\n");
	pa_parse_protocol_data(pa);

	dbg_str(DBG_IMPORTANT,"print list for each after parse");
	pfs_print_list_for_each(pa->pa_list_head_p);

/*
 *    dbg_str(DBG_DETAIL,"pa_destroy_protocol_analyzer");
 *    pa_destroy_protocol_analyzer(pa);
 *
 *    dbg_str(DBG_DETAIL,"pfs_destroy_protocol_format_set");
 *    pfs_destroy_protocol_format_set(pfs_p);
 */

	return ret;
}

