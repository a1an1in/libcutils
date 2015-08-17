/*
 * =====================================================================================
 *
 *       Filename:  pdt_proto_generate_data.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/10/2015 09:09:38 PM
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
#include <math.h>
#include "libdbg/debug.h"
#include "libproto_analyzer/protocol_format_set.h"
#include "libproto_analyzer/protocol_analyzer.h"
#include "libproto_analyzer/pdt_proto_format.h"

void pdt_init_proto_3008(uint16_t lab,uint16_t slot_info,
		uint16_t sync_info,uint8_t tact_info,uint8_t slc_info,
		uint32_t slc_data,uint8_t sync_index,uint8_t st_info,
		uint8_t dc_info,uint8_t *dc_data ,struct protocol_analyzer_s *pa)
{
	pa_set_value(PROTO_NUM,0x3008,pa);
	pa_set_value(LAB,lab,pa);
	pa_set_value(SLOT_INFO,slot_info,pa);
	pa_set_value(SYNC_INFO,sync_info,pa);
	pa_set_value(TACT_INFO,tact_info,pa);
	pa_set_variable_length_value(SLC_INFO,slc_info,pa);
	pa_set_value(SLC_DATA,slc_data,pa);
	pa_set_value(SYNC_INDEX,sync_index,pa);
	pa_set_value(ST_INFO,st_info,pa);
	pa_set_variable_length_value(DC_INFO,dc_info,pa);
	pa_set_buf(DC_DATA,dc_data,dc_info,pa);
}
