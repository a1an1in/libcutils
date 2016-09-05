/*
 * Copyright (c) 2008-2012 Niels Provos, Nick Mathewson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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
 */
#ifndef _EVTHREAD_INTERNAL_H_
#define _EVTHREAD_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define EVTHREAD_GET_ID()	1
#define EVTHREAD_ALLOC_LOCK(lockvar, locktype) _EVUTIL_NIL_STMT
#define EVTHREAD_FREE_LOCK(lockvar, locktype) _EVUTIL_NIL_STMT
#define EVLOCK_LOCK(lockvar, mode) _EVUTIL_NIL_STMT
#define EVLOCK_UNLOCK(lockvar, mode) _EVUTIL_NIL_STMT
#define EVLOCK_LOCK2(lock1,lock2,mode1,mode2) _EVUTIL_NIL_STMT
#define EVLOCK_UNLOCK2(lock1,lock2,mode1,mode2) _EVUTIL_NIL_STMT
#define EVBASE_IN_THREAD(base)	1
#define EVBASE_NEED_NOTIFY(base) 0
#define EVBASE_ACQUIRE_LOCK(base, lock) _EVUTIL_NIL_STMT
#define EVBASE_RELEASE_LOCK(base, lock) _EVUTIL_NIL_STMT
#define EVLOCK_ASSERT_LOCKED(lock) _EVUTIL_NIL_STMT
#define EVLOCK_TRY_LOCK(lock) 1
#define EVTHREAD_ALLOC_COND(condvar) _EVUTIL_NIL_STMT
#define EVTHREAD_FREE_COND(cond) _EVUTIL_NIL_STMT
#define EVTHREAD_COND_SIGNAL(cond) _EVUTIL_NIL_STMT
#define EVTHREAD_COND_BROADCAST(cond) _EVUTIL_NIL_STMT
#define EVTHREAD_COND_WAIT(cond, lock) _EVUTIL_NIL_STMT
#define EVTHREAD_COND_WAIT_TIMED(cond, lock, howlong) _EVUTIL_NIL_STMT
#define EVTHREAD_LOCKING_ENABLED() 0


#ifdef __cplusplus
}
#endif

#endif /* _EVTHREAD_INTERNAL_H_ */
