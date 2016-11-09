/*
 * =====================================================================================
 *
 *       Filename:  libdbg_register_modules.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/13/2015 07:54:53 PM
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
extern void console_print_regester();
extern void log_print_regester();

__attribute__((constructor(101))) void libdbg_register_modules()
{
    printf("register libdbg modules start,prior 101\n");
	console_print_regester();
	//network_print_regester();
	log_print_regester();
    printf("register libdbg modules end\n");
}
