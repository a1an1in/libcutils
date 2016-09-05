/*
 * XXX This sample code was once meant to show how to use the basic Libevent
 * interfaces, but it never worked on non-Unix platforms, and some of the
 * interfaces have changed since it was first written.  It should probably
 * be removed or replaced with something better.
 *
 * Compile with:
 * cc -I/usr/local/include -o event-test event-test.c -L/usr/local/lib -levent
 */

#include <event2/event-config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <event.h>

static void
fifo_read(evutil_socket_t fd, short event, void *arg)
{
	char buf[255];
	int len;
	struct event *ev = arg;

	/* Reschedule this event */
	event_add(ev, NULL);

	fprintf(stderr, "fifo_read called with fd: %d, event: %d, arg: %p\n",
	    (int)fd, event, arg);
	len = read(fd, buf, sizeof(buf) - 1);

	if (len == -1) {
		perror("read");
		return;
	} else if (len == 0) {
		fprintf(stderr, "Connection closed\n");
		return;
	}

	buf[len] = '\0';
	fprintf(stdout, "Read: %s\n", buf);
}
int test_io()
{
	struct event evfifo;
	struct stat st;
	const char *fifo = "event.fifo";
	int socket;
	struct event_base *event_base;

	if (lstat(fifo, &st) == 0) {
		if ((st.st_mode & S_IFMT) == S_IFREG) {
			errno = EEXIST;
			perror("lstat");
			exit(1);
		}
	}

	unlink(fifo);
	if (mkfifo(fifo, 0600) == -1) {
		perror("mkfifo");
		exit(1);
	}

	/* Linux pipes are broken, we need O_RDWR instead of O_RDONLY */
	socket = open(fifo, O_RDWR | O_NONBLOCK, 0);

	if (socket == -1) {
		perror("open");
		exit(1);
	}

	fprintf(stderr, "Write data to %s\n", fifo);
	/* Initalize the event library */
	event_base = event_base_new();

	/* Initalize one event */
	event_assign(&evfifo,event_base, socket, EV_READ, fifo_read, &evfifo);

	/* Add it to the active events, without a timeout */
	event_add(&evfifo, NULL);

	event_base_dispatch(event_base);

	return (0);
}

