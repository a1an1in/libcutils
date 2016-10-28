#ifndef __BUS_H__
#define __BUS_H__

#include <libipc/net/client.h>

typedef struct bus_s{
    allocator_t *allocator;
    client_t *client;
    char *client_sk_type;
    char *server_host;
    char *server_srv;
}bus_t;

#endif
