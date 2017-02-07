/*
 * Compile with:
 * cc -I/usr/local/include -o signal-test \
 *   signal-test.c -L/usr/local/lib -levent
 */

#include <sys/types.h>

#include <event2/event-config.h>

#include <sys/stat.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <event.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
#ifdef _EVENT___func__
#define __func__ _EVENT___func__
#endif

#include <libdbg/debug.h>

int called = 0;

static void
signal_cb(evutil_socket_t fd, short event, void *arg)
{
    struct event *signal = arg;

    printf("%s: got signal %d\n", __func__, EVENT_SIGNAL(signal));

    if (called >= 2)
        event_del(signal);

    called++;
}

int test_signal()
{
    struct event signal_int;
    struct event_base* base;

    /* Initalize the event library */
    base = event_base_new();

    /* Initalize one event */
    event_assign(&signal_int, base, SIGINT, EV_SIGNAL|EV_PERSIST, signal_cb,
        &signal_int);

    event_add(&signal_int, NULL);

    event_base_dispatch(base);
    event_base_free(base);

    return (0);
}

void * signal_thread_callback_fn(void *arg)
{
    struct event signal_int;
    struct event_base* base;

    /* Initalize the event library */
    base = event_base_new();

    /* Initalize one event */
    event_assign(&signal_int, base, SIGINT, EV_SIGNAL|EV_PERSIST, signal_cb,
        &signal_int);

    event_add(&signal_int, NULL);

    event_base_dispatch(base);
    event_base_free(base);

    return (0);
}
//test if we create a thread,then the vsz will enlarge too much, and the test end is yes
int test_signal2()
{
    pthread_t tid;

    pthread_create(&tid,NULL,signal_thread_callback_fn,NULL);

    pause();//this pause will be breaked by signal 
    dbg_str(DBG_WARNNING,"pause breaked");

    return (0);
}
