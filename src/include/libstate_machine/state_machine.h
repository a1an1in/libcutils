#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__
#include <libdata_structure/vector.h>
#include <libconcurrent/io_user.h>
#include <libconcurrent/tmr_user.h>

#define STATE_MACHINE_ENTRY_NAME_MAX_LEN 50

typedef struct state_machine_s{
	vector_t *vector;
    allocator_t *allocator;
    int entry_num;
    int read_notify_fd;
    int write_notify_fd;
    io_user_t *notifier;
    int current_state;
    int last_state;
    void *base;
}state_machine_t;

typedef struct state_entry_s{
    void (*action_callback)(state_machine_t *s,void *opaque);
	void (*process_timer_task_cb)(void *tmr);
    struct timeval tv;
    tmr_user_t *timer;
    state_machine_t *state_machine;
    char entry_name[STATE_MACHINE_ENTRY_NAME_MAX_LEN];
}state_entry_t;

typedef struct state_entry_config_s{
     char entry_name[STATE_MACHINE_ENTRY_NAME_MAX_LEN];
     void (*action_callback)(state_machine_t *s,void *opaque);
     void (*process_timer_task_cb)(void *tmr);
     time_t tv_sec;
     suseconds_t tv_usec;
}state_entry_config_t;


state_machine_t *state_machine_create(allocator_t *allocator);
void state_machine_register_state_entry(state_machine_t *s, state_entry_t *e);
int state_machine_setup_entry_timer(state_machine_t *s,uint8_t state);
int state_machine_stop_entry_timer(state_machine_t *s,uint8_t state);
void
state_machine_change_state(state_machine_t *s, int state);
state_machine_t *state_machine(allocator_t *allocator, state_entry_config_t *config,void *base);
int state_machine_get_cur_state(state_machine_t *s);
void state_machine_change_state_force(state_machine_t *s, int state);
state_machine_t *state_machine_destroy(state_machine_t * s);
void test_state_machine();

#endif
