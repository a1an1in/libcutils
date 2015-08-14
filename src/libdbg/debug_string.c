/*
 * =====================================================================================
 *
 *       Filename:  debug_string.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/25/2015 11:37:41 AM
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
#include "libdbg/debug_string.h"

size_t debug_string_buf_to_str(uint8_t *buf_addr,size_t buf_len,char *str,size_t str_len)
{
	size_t i;
	size_t offset=0;

	for(i = 0; i < buf_len; i++){
		if(str_len - offset <= 4){
			break;
		}
		offset += snprintf(str+offset, str_len-offset, "%x-", buf_addr[i]);
	}
	return offset;
}
int debug_string_itoa(int val, char* buf,int radix)
{
	char* p;
	unsigned int a; //every digit
	int len;
	char* b; //start of the digit char
	char temp;
	unsigned int u;

	p = buf;
	if (val < 0) {
		*p++ = '-';
		val = 0 - val;
	}
	u = (unsigned int)val;
	b = p;
	do{
		a = u % radix;
		u /= radix;
		*p++ = a + '0';
	} while (u > 0);

	len = (int)(p - buf);
	*p-- = 0;

	do{
		temp = *p;
		*p = *b;
		*b = temp;
		--p;
		++b;
	} while (b < p);

	return len;
}

