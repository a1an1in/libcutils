#ifndef __CONTAINER_RBTREE_MAP__
#define __CONTAINER_RBTREE_MAP__

#include "basic_types.h"
#include "rbtree.h"

#ifndef __KEY_CMP_FPT__
#define __KEY_CMP_FPT__
typedef int (*key_cmp_fpt)(void *key1,void *key2,uint32_t key_size);
#endif
struct rbtree_map_node {
  	struct rb_node node;
	uint8_t value_pos;
  	char key[1];
};
typedef struct rbtree_map_priv_info{
	uint32_t data_size;
	uint32_t key_size;
	uint32_t value_size;
	struct rb_root *tree_root;
	key_cmp_fpt key_cmp_func;
}rbtree_map_priv_t;

int container_rbtree_map_register();
#endif
