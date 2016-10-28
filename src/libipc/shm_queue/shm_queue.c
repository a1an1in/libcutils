#include <stdio.h>
#include <libipc/shm_queue/shm_queue.h>
#include <libdbg/debug.h>
#include <liballoc/allocator.h>

/*
 *typedef struct shm_queue_s{
 *    uint32_t fifo_fd;
 *    share_mem_t *shm;
 *}shm_queue_t;
 */
shm_queue_t *shm_queue_create(allocator_t *allocator)
{
    shm_queue_t *que;

    que = (shm_queue_t *)allocator_mem_alloc(allocator,sizeof(shm_queue_t));
    if(que == NULL){
        dbg_str(DBG_DETAIL,"allocator_mem_alloc");
        return que;
    }

    return que;
}

int shm_queue_set(shm_queue_t *que)
{
    int ret = 0;


    return ret;
}
