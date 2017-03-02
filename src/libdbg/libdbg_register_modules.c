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
#include <libdbg/debug.h>
#include <attrib_priority.h>

extern void console_print_regester();
extern void log_print_regester();

__attribute__((constructor(LIBDBG_REGISTER_MODULES_ATTRIB_PRIORITY))) 
void libdbg_register_modules()
{
    ATTRIB_PRINT("constructor LIBDBG_REGISTER_MODULES_ATTRIB_PRIORITY=%d,register libdbg modules\n",
                 LIBDBG_REGISTER_MODULES_ATTRIB_PRIORITY);
    console_print_regester();
    //network_print_regester();
    log_print_regester();
}
