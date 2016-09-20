#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__
#include <libdata_structure/vector.h>
#include <libconcurrent/io_user.h>
#include <libconcurrent/tmr_user.h>

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
    void (*timer_event_callback)(int fd, short event, void *arg);
    struct timeval tv;
    tmr_user_t *timer;
    state_machine_t *state_machine;
#define MAX_ENTRY_NAME_LEN 50
    char entry_name[MAX_ENTRY_NAME_LEN];
#undef MAX_ENTRY_NAME_LEN 
}state_entry_t;

typedef struct state_entry_config_s{
 #define MAX_ENTRY_NAME_LEN 50
     char entry_name[MAX_ENTRY_NAME_LEN];
 #undef MAX_ENTRY_NAME_LEN 
     void (*action_callback)(state_machine_t *s,void *opaque);
     void (*timer_event_callback)(int fd, short event, void *arg);
     time_t tv_sec;
     suseconds_t tv_usec;
}state_entry_config_t;


state_machine_t *state_machine_create(allocator_t *allocator);
int state_machine_init(state_machine_t *s,
        void (*notification_event_handler)(int fd, short event, void *arg),
        int entry_num);
state_entry_t *init_state_entry(state_entry_t *e,
        state_machine_t *s,
        void (*action_callback)(state_machine_t *s,void *opaque),
        void (*timer_event_callback)(int fd, short event, void *arg),
        time_t       tv_sec,     /* seconds */
        suseconds_t   tv_usec, /* microseconds */
        char *entry_name);
state_entry_t *state_machine_construct_state_entry(state_machine_t *s,
        void (*action_callback)(state_machine_t *s,void *opaque),
        void (*timer_event_callback)(int fd, short event, void *arg),
        time_t       tv_sec,     /* seconds */
        suseconds_t   tv_usec, /* microseconds */
        char *entry_name);
void
state_machine_register_state_entry(state_machine_t *s, state_entry_t *e);
int state_machine_setup_entry_timer(state_machine_t *s,uint8_t state);
int state_machine_stop_entry_timer(state_machine_t *s,uint8_t state);
void
state_machine_change_state(state_machine_t *s, int state);
state_machine_t *state_machine(allocator_t *allocator, state_entry_config_t *config);
void test_state_machine();

#endif
