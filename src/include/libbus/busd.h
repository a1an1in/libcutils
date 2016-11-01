#ifndef __BUSD_H__
#define __BUSD_H__

#include <libipc/net/server.h>
#include <libdata_structure/vector.h>
#include <libblob/blob.h>


enum {
	BUSD_ID,
	BUSD_OBJNAME,
	BUSD_METHORDS,
	__BUSD_MAX
};

typedef struct busd_s{
    allocator_t *allocator;
    server_t *server;
    char *server_sk_type;
    char *server_host;
    char *server_srv;
}busd_t;

struct busd_object_method_arg_s{
	char *name;
	uint8_t type;
};
struct busd_object_method {
	char *name;
	llist_t *args;
};

struct busd_object {
	char *name;
	uint32_t id;
	vector_t *methods;
};

typedef int (*busd_cmd_callback)(busd_t *busd,  struct blob_attr_s **attr);

#endif
