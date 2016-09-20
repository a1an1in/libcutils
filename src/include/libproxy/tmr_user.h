#ifndef __EV_TIMER_H__
#define __EV_TIMER_H__
typedef struct tmr_user_s{
	struct event event;
    allocator_t *allocator;
    void (*tmr_event_handler)(int fd, short event, void *arg);
    void *opaque;
    int32_t tmr_user_fd;
	concurrent_master_t *master;
	struct timeval tv;
    uint32_t flags;
}tmr_user_t;
#endif
