/*
 * =====================================================================================
 *
 *       Filename:  libcre_register_modules.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/24/2015 04:27:25 PM
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
#include "libcre/sync_lock/sync_lock.h"

__attribute__((constructor(101))) void libcre_register_modules()
{
	sync_lock_register_modules();
}
