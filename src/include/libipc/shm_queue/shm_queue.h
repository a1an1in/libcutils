#ifndef __SHM_RWQUEUE_H__
#define __SHM_RWQUEUE_H__

#include <basic_types.h>
#include <libipc/share_mem/shm.h>

typedef struct shm_queue_s{
#define SHM_QUEUE_MAX_NAME_LEN 50
    uint32_t fifo_fd;
    char fifo_name[SHM_QUEUE_MAX_NAME_LEN];

    share_mem_t *shm;
    char shm_name[SHM_QUEUE_MAX_NAME_LEN];
    uint32_t shm_size;
    uint8_t shm_type;
#undef SHM_QUEUE_MAX_NAME_LEN
}shm_queue_t;

#endif
