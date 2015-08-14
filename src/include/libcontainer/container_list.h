#ifndef __CONTAINER_LIST__
#define __CONTAINER_LIST__

#include "basic_types.h"
#include "list.h"

struct container_list{
	struct list_head list_head;
	uint8_t data[1];
};
typedef struct list_priv_info{
	uint32_t data_size; //the size of data in list
}list_priv_t;

int container_list_register();
#endif
