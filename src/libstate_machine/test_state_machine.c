#include <fcntl.h>              
#include <unistd.h>
#include <libdbg/debug.h>
#include <libdata_structure/vector.h>
#include <libconcurrent/io_user.h>
#include <libconcurrent/tmr_user.h>
#include <libstate_machine/state_machine.h>

static void init_state_timeoutworkfunc_cb(void *arg)
{
    dbg_str(SM_DETAIL,"init_state_timeout");
}

static void init_state_changing_workfunc_cb(state_machine_t *s,void *opaque)
{
    dbg_str(SM_VIP,"init_state_action");
    state_machine_change_state(s, 2);
}

static void applying_state_timeoutworkfunc_cb(void *arg)
{
    tmr_user_t *timer = (tmr_user_t *)arg;
    state_machine_t *s = (state_machine_t *)timer->opaque;
    dbg_str(SM_DETAIL,"applying_state_timeout");
    state_machine_change_state(s, 4);
}

static void applying_state_changing_workfunc_cb(state_machine_t *s,void *opaque)
{
    dbg_str(SM_DETAIL,"applying_state_action");
}

static void waiting_state_timeoutworkfunc_cb(void *arg)
{
    dbg_str(SM_DETAIL,"waiting_state_timeout");
}

static void waiting_state_changing_workfunc_cb(state_machine_t *s,void *opaque)
{
    dbg_str(SM_DETAIL,"waiting_state_action");
}

static void scanning_state_timeoutworkfunc_cb(void *arg)
{
    tmr_user_t *timer = (tmr_user_t *)arg;
    state_machine_t *s = (state_machine_t *)timer->opaque;
    dbg_str(SM_DETAIL,"scanning_state_timeout");
    state_machine_change_state(s, 5);
}

static void scanning_state_changing_workfunc_cb(state_machine_t *s,void *opaque)
{
    dbg_str(SM_DETAIL,"scanning_state_action");
}

static void scanned_state_timeoutworkfunc_cb(void *arg)
{
    tmr_user_t *timer = (tmr_user_t *)arg;
    state_machine_t *s = (state_machine_t *)timer->opaque;
    dbg_str(SM_DETAIL,"scanned_state_timeout");

    state_machine_change_state(s, 2);
}

static void scanned_state_changing_workfunc_cb(state_machine_t *s,void *opaque)
{
    dbg_str(SM_DETAIL,"scanned_state_action");
}

static state_entry_config_t entry_config[]={
    {"uninited", NULL,NULL, 1, 0},
    {"init",     init_state_changing_workfunc_cb,init_state_timeoutworkfunc_cb, 1, 0},
    {"applying", applying_state_changing_workfunc_cb,applying_state_timeoutworkfunc_cb, 2, 0},
    {"waiting",  waiting_state_changing_workfunc_cb,waiting_state_timeoutworkfunc_cb, 15, 0},
    {"scanning", scanning_state_changing_workfunc_cb,scanning_state_timeoutworkfunc_cb, 13, 0},
    {"scaned",   scanned_state_changing_workfunc_cb,scanned_state_timeoutworkfunc_cb, 60, 0},
    {"",NULL,NULL,0,0},
};

void test_state_machine()
{
	allocator_t *allocator = allocator_get_default_alloc();
    state_machine_t *s;

    printf("test state_machine\n");

    s = state_machine(allocator, entry_config,NULL);
    state_machine_change_state(s, 1);

}
