#include <ctype.h>
#include <pthread.h>
#include <math.h>
#include "libdbg/debug.h"
#include "liballoc/allocator.h"
#include "libproto_analyzer/protocol_format_set.h"
#include "libproto_analyzer/protocol_analyzer.h"
#include "pdt_drv_proto_format.h"

debugger_t *debugger_gp;
allocator_t *allocator;

int test_pdt_proto_3008(protocol_format_set_t *pfs_p,
		allocator_t *allocator)
{
	int ret = 0;
	struct protocol_analyzer_s *pa;
	uint8_t proto_data2[23] ={
		0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,
		0xb,0xc,0xd,0xe,0xf,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18
	};


	/*set and parse test*/
	pa = pa_create_protocol_analyzer(allocator);
	pa_init_protocol_analyzer(0x3008, pfs_p, pa);
	/*
	 *pa = proto_analyer_admin_get_pa(pa_admin,0x3008);
	 */

	pa_set_value(PROTO_NUM,0x3008,pa);
	pa_set_value(LAB,0x1834,pa);
	pa_set_value(SLOT_INFO,0x1234,pa);
	pa_set_value(SYNC_INFO,0,pa);
	pa_set_value(TACT_INFO,2,pa);
	pa_set_variable_length_value(SLC_INFO,4,pa);
	pa_set_value(SLC_DATA,0xaabbccdd,pa);
	pa_set_value(SYNC_INDEX,0x24,pa);
	pa_set_value(ST_INFO,0x32,pa);
	pa_set_variable_length_value(DC_INFO,23,pa);
	pa_set_buf(DC_DATA,proto_data2,23,pa);

	dbg_str(DBG_IMPORTANT,"print list for each,orinal data");
	pfs_print_list_for_each(pa->pa_list_head_p);

	pa_generate_protocol_data(pa);

	dbg_str(DBG_DETAIL,"pa_parse_protocol_data");
	pa_parse_protocol_data(pa);

	dbg_str(DBG_IMPORTANT,"print list for each after parse");
	pfs_print_list_for_each(pa->pa_list_head_p);

	dbg_str(DBG_DETAIL,"pa_destroy_protocol_analyzer");
	pa_destroy_protocol_analyzer(pa);

	dbg_str(DBG_DETAIL,"pfs_destroy_protocol_format_set");
	pfs_destroy_protocol_format_set(pfs_p);

	return ret;
}
int test_pdt_proto_3019(protocol_format_set_t *pfs_p,
		allocator_t *allocator)
{
	int ret = 0;
	struct protocol_analyzer_s *pa;
	uint8_t proto_data2[23] ={
		0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,
		0xb,0xc,0xd,0xe,0xf,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18
	};


	/*set and parse test*/
	pa = pa_create_protocol_analyzer(allocator);
	pa_init_protocol_analyzer(0x3019, pfs_p, pa);

	/*
	 *PFS_SET_PROTO_INFO( PROTO_NUM,    "0", "0", "2", "8",  NULL,     hl_head);
	 *PFS_SET_PROTO_INFO( LAB,          "2", "0", "2", "8",  NULL,     hl_head);
	 *PFS_SET_PROTO_INFO( SLOT_INFO,    "6", "0", "2", "8",  NULL,     hl_head);
	 *PFS_SET_PROTO_INFO( SYNC_INFO,    "8", "0", "2", "8",  NULL,     hl_head);
	 *PFS_SET_PROTO_INFO( ST_INFO,      "10","0", "1", "8",  NULL,     hl_head);
	 *PFS_SET_PROTO_INFO( DC_INFO,      "11","0", "1", "8",  NULL,     hl_head);
	 *PFS_SET_PROTO_INFO( DC_DATA,      "12","0", "0", "8",  DC_INFO,  hl_head);
	 */
	pa_set_value(PROTO_NUM,0x3019,pa);
	pa_set_value(LAB,0x1834,pa);
	pa_set_value(SLOT_INFO,0x1234,pa);
	pa_set_value(SYNC_INFO,0,pa);
	pa_set_value(ST_INFO,0x32,pa);
	pa_set_variable_length_value(DC_INFO,23,pa);
	pa_set_buf(DC_DATA,proto_data2,23,pa);

	dbg_str(DBG_IMPORTANT,"print list for each,orinal data");
	pfs_print_list_for_each(pa->pa_list_head_p);

	pa_generate_protocol_data(pa);

	dbg_str(DBG_DETAIL,"pa_parse_protocol_data");
	pa_parse_protocol_data(pa);

	dbg_str(DBG_IMPORTANT,"print list for each after parse");
	pfs_print_list_for_each(pa->pa_list_head_p);

	dbg_str(DBG_DETAIL,"pa_destroy_protocol_analyzer");
	pa_destroy_protocol_analyzer(pa);

	dbg_str(DBG_DETAIL,"pfs_destroy_protocol_format_set");
	pfs_destroy_protocol_format_set(pfs_p);

	return ret;

}
void pfs_set_pdt_drv_proto_format(protocol_format_set_t *pfs_p)
{
	pfs_set_proto_format_3008(pfs_p);
	pfs_set_proto_format_3019(pfs_p);
}
int test_pdt_proto_analyzer()
{
	protocol_format_set_t *pfs_p;
	uint32_t ret = 0;

	allocator = allocator_creator(ALLOCATOR_TYPE_SYS_MALLOC);

	pfs_p = pfs_create_proto_format_set(allocator);
	init_proto_format_set(0x3000,100,pfs_p);
	pfs_set_pdt_drv_proto_format(pfs_p);


	/*
	 *test_pdt_proto_3008(pfs_p,allocator);
	 */
	test_pdt_proto_3019(pfs_p,allocator);

	return ret;
}

