/*
 * =====================================================================================
 *
 *       Filename:  linux_user_mode_mutex.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/24/2015 03:32:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
/*  
 * Copyright (c) 2015-2020 alan lin <a1an1in@sina.com>
 *  
 *  
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cutils_re.h"
#include "libdbg/debug.h"

#ifdef WINDOWS_USER_MODE

int windows_mutex_init(struct sync_lock_s *slock)
{
	printf("not support yet\n");
}
int windows_mutex_lock(struct sync_lock_s *slock,void *arg)
{
	printf("not support yet\n");
}
int windows_mutex_trylock(struct sync_lock_s *slock,void *arg)
{
	printf("not support yet\n");
}
int windows_mutex_unlock(struct sync_lock_s *slock)
{
	printf("not support yet\n");
}
int windows_mutex_lock_destroy(struct sync_lock_s *slock)
{
	printf("not support yet\n");
}
int  windows_user_mode_mutex_register(){
	sync_lock_module_t slm = {
		.name = "pthread_mutex",
		.sync_lock_type = WINDOWS_MUTEX_LOCK,
		.sl_ops = {
			.sync_lock_init    = windows_mutex_init,
			.sync_lock         = windows_mutex_lock,
			.sync_trylock      = windows_mutex_trylock,
			.sync_unlock       = windows_mutex_unlock,
			.sync_lock_destroy = windows_mutex_lock_destroy,
		},
	};
	memcpy(&sync_lock_modules[WINDOWS_MUTEX_LOCK],&slm,sizeof(sync_lock_module_t));
	return 0;
}

#endif
