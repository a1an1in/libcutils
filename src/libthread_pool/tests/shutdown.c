#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "libthread_pool/threadpool.h"

#define THREAD 4
#define SIZE   8192

static threadpool_t *pool;
static int left;
static pthread_mutex_t lock;
static int error;

static void dummy_task(void *arg) {
    usleep(100);
    pthread_mutex_lock(&lock);
    left--;
    pthread_mutex_unlock(&lock);
}

int test_shutdown()
{
    int i;

    pthread_mutex_init(&lock, NULL);

    /* Testing immediate shutdown */
    left = SIZE;
    pool = threadpool_create(THREAD, SIZE, 0);
    for(i = 0; i < SIZE; i++) {
        assert(threadpool_add(pool, &dummy_task, NULL, 0) == 0);
    }
    assert(threadpool_destroy(pool, 0) == 0);
	printf("left=%d\n",left);
    assert(left > 0);

    /* Testing graceful shutdown */
    left = SIZE;
    pool = threadpool_create(THREAD, SIZE, 0);
    for(i = 0; i < SIZE; i++) {
        assert(threadpool_add(pool, &dummy_task, NULL, 0) == 0);
    }
    assert(threadpool_destroy(pool, threadpool_graceful) == 0);
	printf("left=%d\n",left);
    assert(left == 0);

    pthread_mutex_destroy(&lock);

    return 0;
}
