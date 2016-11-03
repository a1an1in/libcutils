#ifndef __BUSD_H__
#define __BUSD_H__

#include <libipc/net/server.h>
#include <libdata_structure/vector.h>
#include <libdata_structure/hash_list.h>
#include <libblob/blob.h>

enum {
	BUSD_ID,
	BUSD_OBJNAME,
	BUSD_METHORDS,
    BUSD_INVOKE_KEY,
	BUSD_INVOKE_METHORD,
    BUSD_INVOKE_ARGS,
	__BUSD_MAX
};

typedef struct busd_task_s{
    int fd;
}busd_task_t;

typedef struct busd_s{
    allocator_t *allocator;
    server_t *server;
    char *server_sk_type;
    char *server_host;
    char *server_srv;

	hash_map_t *obj_hmap;
    pair_t *pair;
    uint8_t key_size;
    uint8_t bucket_size;
}busd_t;

struct busd_object_method_arg_s{
	char *name;
	uint8_t type;
};
struct busd_object_method {
	char *name;
	llist_t *args;
};

typedef struct busd_object {
	char *name;
	uint32_t id;
	vector_t *methods;
    uint8_t fd;
}busd_object_t;

typedef int (*busd_cmd_callback)(busd_t *busd,  struct blob_attr_s **attr,int fd);

#endif
