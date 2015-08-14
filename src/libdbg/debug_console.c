/*
 * =====================================================================================
 *
 *       Filename:  console.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/24/2015 03:49:44 AM
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
#include <string.h>
#include <unistd.h>
#include "libdbg/debug.h"


/*-----------------------------------------------------------------------------
 *功能： 打印有颜色的字符
 *-----------------------------------------------------------------------------*/
int console_print_print_str_vl(debugger_t *debugger,
		size_t level,const char *fmt,va_list vl)
{
#define MAX_STR_LEN 1024
	char dest[MAX_STR_LEN];
	size_t offset=0 , ret = 0;
	int reverse_color_flag,background_color,front_color,high_light_flag;
	char high_light_value[MAX_STR_LEN];
	char reverse_color_value[MAX_STR_LEN];
	size_t color_value;

	color_value = debugger_get_level_color(debugger,level);

	memset(dest,'\0',MAX_STR_LEN);
	offset = vsnprintf(dest,MAX_STR_LEN,fmt,vl);

	reverse_color_flag = color_value&0x1;
	background_color  = ((color_value&0x70)>>4);
	front_color  = ((color_value&0xe)>>1);
	high_light_flag  = ((color_value>>7)&1);

	if(high_light_flag == 1){
		strcpy(high_light_value,"\33[1m");
	}else{
		strcpy(high_light_value,"");
	}
	if(reverse_color_flag == 1){
		strcpy(reverse_color_value,"\33[7m");
	}else{
		strcpy(reverse_color_value,"");
	}
	if((color_value & 0xfe) == 0xe & high_light_flag == 0){
		printf("\33[0m\33[%dm\33[%dm",49,30);
		printf("%s",dest);
		printf("\33[0m\033[K");
	}else{
		printf("\33[0m%s%s\33[%dm\33[%dm",high_light_value,reverse_color_value,background_color+40,front_color+30);
		printf("%s",dest);
		printf("\33[0m\033[K");
	}
	printf("\n");
	return strlen(dest);
#undef MAX_STR_LEN 

	return ret;
}
int console_print_print(debugger_t *debugger,size_t level,const char *fmt,...)
{
	int ret;
	va_list ap;

	va_start(ap,fmt);
	ret = console_print_print_str_vl(debugger,level,fmt,ap);
	va_end(ap);

	return ret;
}
void console_print_regester()
{
	debugger_module_t dm={
		.dbg_ops ={
			.dbg_string_vl = console_print_print_str_vl,
			.dbg_string    = console_print_print,
			.init          = NULL,
			.destroy       = NULL,
		}
	};
	memcpy(&debugger_modules[DEBUGGER_TYPE_CONSOLE],&dm,sizeof(debugger_module_t));
}
