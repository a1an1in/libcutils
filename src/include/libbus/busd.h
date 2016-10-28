#ifndef __BUSD_H__
#define __BUSD_H__

#include <libipc/net/server.h>

typedef struct busd_s{
    allocator_t *allocator;
    server_t *server;
    char *server_sk_type;
    char *server_host;
    char *server_srv;
}busd_t;

#endif
