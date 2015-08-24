#ifndef __CONTAINOR_H__
#define __CONTAINOR_H__
#include "libcontainer/inc_files.h"
#include "libcontainer/container_hash_map.h"
#include "libcontainer/container_list.h"
#include "libcontainer/container_rbtree_map.h"
#include "libcontainer/container_vector.h"
#include "libcre/sync_lock/sync_lock.h"

#define container_for_each_safe(it, next, ct) \
	for (it = container_begin(ct), next = iterator_next(it);\
			!iterator_equal(it,container_end(ct));\
			it = next, next = iterator_next(it))

#define container_for_each(it, ct) \
	for (it = container_begin(ct); !iterator_equal(it,container_end(ct)); it = iterator_next(it))

enum container_type{
	CONTAINER_TYPE_VECTOR = 0,
	CONTAINER_TYPE_LIST,
	CONTAINER_TYPE_RBTREE_MAP,
	CONTAINER_TYPE_HASH_MAP,
	CONTAINER_TYPE_MAX_NUM
};

typedef struct pair{
	uint16_t data_len;
	uint32_t key_len;
	uint32_t value_len;
	uint8_t data[1];
}pair_t;

static inline pair_t * create_pair(int key_len,int value_len)
{
	pair_t *p;
	p = (pair_t *)malloc(sizeof(pair_t) + key_len + value_len);
	p->key_len = key_len;
	p->value_len = value_len;
	p->data_len = key_len + value_len;
	return p;
}
static inline void make_pair(pair_t *p,void *key,void *value)
{
	memcpy(p->data,key,p->key_len);
	memcpy(p->data + p->key_len,value,p->value_len);
	dbg_buf(DBG_CONTAINER_DETAIL,"make pair:",p->data,p->data_len);
}
static inline int destroy_pair(pair_t *p)
{
	free(p);
	p = NULL;
	return 0;
}

typedef struct iterator{
	union{
		struct list_head *list_head_p;
		struct rb_node *rb_node_p;
		hash_map_pos_t hash_map_pos;
		uint32_t vector_pos;
	}pos;
	 struct container *container_p;
}iterator_t;


typedef struct container{
#	define COTAINOR_NAME_MAX_LEN 40
	char name[COTAINOR_NAME_MAX_LEN];
	uint8_t container_type;
	allocator_t *allocator;
	struct container_operations *c_ops_p;
	struct iterator_operations *it_ops_p;
	struct iterator begin,end,cur;
	sync_lock_t head_lock;    
	union{
		list_priv_t list_priv;
		rbtree_map_priv_t rbtree_map_priv;
		hash_map_priv_t hash_map_priv;
		vector_priv_t vector_priv;
	}priv;
#	undef COTAINOR_NAME_MAX_LEN
}container_t;

struct container_operations{
	int (*init)(container_t *);
	int (*map_init)(container_t *ct, uint32_t key_size, uint32_t value_size, key_cmp_fpt key_cmp_func);
	int (*push_front)(container_t *ct,void *data);
	int (*push_back)(container_t *ct,void *data);
	int (*pop_front)(container_t *ct);
	int (*pop_back)(container_t *ct);
	iterator_t (*begin)(container_t *ct);
	iterator_t (*end)(container_t *ct);
	int (*insert)(container_t *ct, iterator_t it,void *value);
	int (*map_insert)(container_t *ct,void *value);
	iterator_t (*map_search)(container_t *ct, void *key);
	int (*del)(container_t *ct, iterator_t it);
	int (*destroy)(container_t *ct);
};

struct iterator_operations{
	iterator_t (*next)(iterator_t it);
	iterator_t (*prev)(iterator_t it);
	int (*equal)(iterator_t it1,iterator_t it2);
	void *(*iterator_get_pointer)(iterator_t it);
};

typedef struct container_module{
#	define COTAINOR_NAME_MAX_LEN 20
	char name[COTAINOR_NAME_MAX_LEN];
	uint8_t container_type;
	struct container_operations c_ops;
	struct iterator_operations it_ops;
	struct iterator begin,end;
#	undef COTAINOR_NAME_MAX_LEN
}container_module_t;

extern container_module_t container_modules[CONTAINER_TYPE_MAX_NUM];

pair_t * create_pair(int key_len,int value_len);
void make_pair(pair_t *p,void *key,void *value);
int destroy_pair(pair_t *p);

container_t *container_creator(uint32_t container_type,allocator_t *alloc);
int container_vector_init(container_t *ct,uint32_t data_size,uint32_t capacity);
int container_list_init(container_t *ct,uint32_t data_size);
int container_tree_map_init(container_t *ct, uint32_t key_size, uint32_t value_size, key_cmp_fpt key_cmp_func);
int container_hash_map_init(container_t *ct, uint32_t key_size, uint32_t value_size,uint32_t bucket_size, hash_func_fpt hash_func,key_cmp_fpt key_cmp_func);
int container_push_front(container_t *ct,void *data);
int container_push_back(container_t *ct,void *data);
int container_pop_front(container_t *ct);
int container_pop_back(container_t *ct);
int container_insert(container_t *ct, iterator_t it,void *data);
int container_map_insert(container_t *ct, pair_t *p);
int container_delete(container_t *ct, iterator_t it);
int container_destroy(container_t **ct);

iterator_t container_map_search(container_t *ct, void *key);
iterator_t container_begin(container_t *ct);
iterator_t container_end(container_t *ct);
iterator_t iterator_next(iterator_t it);
iterator_t iterator_prev(iterator_t it);
int iterator_equal(iterator_t it1,iterator_t it2);
void* iterator_get_pointer(iterator_t it);

int container_register();

#endif
