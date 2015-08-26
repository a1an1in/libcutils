/*
 * =====================================================================================
 *
 *       Filename:  sync_lock_register_modules.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/24/2015 03:51:08 PM
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
#include "libcre/libcre.h"
#include "libcre/sync_lock/sync_lock.h"
#include "libcre/sync_lock/posix_thread_mutex.h"
#include "libcre/sync_lock/posix_thread_rwlock.h"
#include "libcre/sync_lock/windows_mutex.h"

void sync_lock_register_modules()
{
	/*
	 *sync_lock_module_t sync_lock_modules[SYNC_LOCK_TYPE_MAX_NUM];
	 */
	/*
	 *memset(&sync_lock_modules[PTHREAD_RWLOCK],0,sizeof(sync_lock_module_t));
	 */
#ifdef UNIX_LIKE_USER_MODE
	linux_user_mode_pthread_mutex_register();
	linux_user_mode_pthread_rwlock_register();
#endif
#ifdef WINDOWS_USER_MODE
	windows_user_mode_mutex_register();
#endif
}

