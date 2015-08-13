/*
 * =====================================================================================
 *
 *       Filename:  test_debugger.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/25/2015 03:54:30 PM
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
#include "debugger/debug.h"

#define MAX_IP_LEN 20
char debug_ip[MAX_IP_LEN] = "127.0.0.1";
#undef MAX_IP_LEN
unsigned int debug_port = 10276;
#define	DBG_NORMAL_ERROR 			DBG_BUSINESS_NORMAL << 8 | DBG_ERROR
#define	DBG_NORMAL_SUC 				DBG_BUSINESS_NORMAL << 8 | DBG_SUC
int main()
{
	int ret = 0;
	uint8_t buf[] = {1,2,3,4,5,6};


	debugger_modules_register();

	debugger_t *debugger_gp = debugger_creator(DEBUGGER_TYPE_CONSOLE);
	debugger_init(debugger_gp);
	/*
	 *debugger_t *debugger_gp = debugger_creator(DEBUGGER_TYPE_NETWORK);
	 *debugger_network_init(debugger_gp,debug_ip,debug_port);
	 */

	/*
	 *debugger_t *debugger_gp = debugger_creator(DEBUGGER_TYPE_LOG);
	 *debugger_log_init(debugger_gp,"log.txt");
	 */


	dbg_str(DBG_NORMAL_ERROR,"hello printf");
	dbg_str(DBG_NORMAL_SUC,"hello printf");

	dbg_buf(DBG_NORMAL_SUC,"buffer string:",buf,6);
	
	return ret;
}

