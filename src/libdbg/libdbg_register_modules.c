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
#include <constructor_priority.h>
#include <libdbg/debug.h>

extern void console_print_regester();
extern void log_print_regester();

__attribute__((constructor(CONSTRUCTOR_PRIORITY_LIBDBG_REGISTER_MODULES))) 
void libdbg_register_modules()
{
    CONSTRUCTOR_PRINT("CONSTRUCTOR_PRIORITY_LIBDBG_REGISTER_MODULES=%d,register libdbg modules\n",
                      CONSTRUCTOR_PRIORITY_LIBDBG_REGISTER_MODULES);
    console_print_regester();
    //network_print_regester();
    log_print_regester();
}
