#ifndef __EV_TIMER_H__
#define __EV_TIMER_H__
typedef struct ev_timer_s{
	struct event event;
    allocator_t *allocator;
    void (*ev_timer_event_handler)(int fd, short event, void *arg);
    void *opaque;
    int32_t ev_timer_fd;
	concurrent_master_t *master;
	struct timeval *tv;
    uint32_t flags;
}ev_timer_t;
#endif
