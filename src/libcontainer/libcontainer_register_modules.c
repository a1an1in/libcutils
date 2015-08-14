#include <stdio.h>
#include "libcontainer/libcontainer_register_modules.h"
#include "libcontainer/inc_files.h"

void libcontainer_register_modules()
{
	container_list_register();
	container_rbtree_map_register();
	container_hash_map_register();
	container_vector_register();
}

