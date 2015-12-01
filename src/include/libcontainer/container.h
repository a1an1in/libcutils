#ifndef __CONTAINOR_H__
#define __CONTAINOR_H__
#include "libcontainer/inc_files.h"
#include "libcre/sync_lock/sync_lock.h"
#include "libdata_structure/link_list.h"
#include "libdata_structure/hash_list.h"
#include "libdata_structure/rbtree_map.h"

#define container_for_each_safe(it,next,ct) \
	iterator_t iterateor_end_;\
	for (	container_begin(ct,&it), iterator_next(&it,&next);\
			!iterator_equal(&it,container_end(ct,&iterateor_end_));\
			it = next, iterator_next(&it,&next))

enum container_type{
	CONTAINER_TYPE_VECTOR = 0,
	CONTAINER_TYPE_LIST,
	CONTAINER_TYPE_RBTREE_MAP,
	CONTAINER_TYPE_HASH_MAP,
	CONTAINER_TYPE_MAX_NUM
};

typedef struct iterator{
	union{
		list_pos_t list_pos;
		hash_map_pos_t hash_pos;
		rbtree_map_pos_t rbtree_pos;
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
	union{
		llist_t *llist;
		hash_map_t *hmap;
		rbtree_map_t *tmap;
	}priv;
#	undef COTAINOR_NAME_MAX_LEN
}container_t;

struct container_operations{
	int (*create)(container_t *ct,uint8_t lock_type);
	int (*init)(container_t *ct,uint32_t data_size);
	/*
	 *int (*map_init)(container_t *ct, uint32_t key_size, uint32_t value_size, key_cmp_fpt key_cmp_func);
	 */
	int (*push_front)(container_t *ct,void *data);
	int (*push_back)(container_t *ct,void *data);
	int (*pop_front)(container_t *ct);
	int (*pop_back)(container_t *ct);
	iterator_t * (*begin)(container_t *ct,iterator_t *it);
	iterator_t * (*end)(container_t *ct,iterator_t *it);
	int (*insert)(container_t *ct, iterator_t *it,void *value);
	int (*map_insert)(container_t *ct,void *value);
	iterator_t * (*map_search)(container_t *ct, void *key,iterator_t *it);
	int (*del)(container_t *ct, iterator_t *it);
	int (*destroy)(container_t *ct);
};

struct iterator_operations{
	iterator_t * (*next)(iterator_t *it,iterator_t *next);
	iterator_t * (*prev)(iterator_t *it,iterator_t *prev);
	int (*equal)(iterator_t *it1,iterator_t *it2);
	void *(*iterator_get_pointer)(iterator_t *it);
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
void libcontainer_register_modules();

static inline container_t *
container_creator(uint32_t container_type,allocator_t *allocator,uint8_t lock_type)
{
	container_t *p;

	p = (container_t *)allocator_mem_alloc(allocator,sizeof(container_t));
	if(p == NULL){
		goto err;
	}

	dbg_str(DBG_CONTAINER_DETAIL,"container_t addr:%p,sizeof_container_struct:%d",p,sizeof(container_t));
	p->container_type = container_type;
	p->allocator = allocator;
	p->c_ops_p = &container_modules[container_type].c_ops;
	p->it_ops_p = &container_modules[container_type].it_ops;

	dbg_str(DBG_CONTAINER_DETAIL,"c_pos_p=%p",p->c_ops_p);
	if(p->c_ops_p == NULL || p->it_ops_p == NULL){
		allocator_mem_free(p->allocator,p);
		p = NULL;
		goto err;
	}else{
		p->c_ops_p->create(p,lock_type);
	}

	dbg_str(DBG_CONTAINER_DETAIL,"run at here");
err:
	return p;
}
static inline int container_init(container_t *ct,uint32_t data_size)
{
	return ct->c_ops_p->init(ct,data_size);
}

static inline int 
container_hash_map_init(container_t *ct, uint32_t key_size,
		uint32_t value_size, uint32_t bucket_size,
		hash_func_fpt hash_func, key_cmp_fpt key_cmp_func)
{
	hash_map_t *hmap =  ct->priv.hmap;

	dbg_str(DBG_CONTAINER_DETAIL,"container_hash_map_init");
	hmap->key_size     = key_size;
	hmap->data_size    = key_size + value_size;
	hmap->hash_func    = hash_func;
	hmap->key_cmp_func = key_cmp_func;
	hmap->bucket_size  = bucket_size;

	return ct->c_ops_p->init(ct,hmap->data_size);
}
static inline int 
container_rbtree_map_init(container_t *ct, uint32_t key_size,
		uint32_t value_size, key_cmp_fpt key_cmp_func)
{
	rbtree_map_t *tmap = ct->priv.tmap;

	dbg_str(DBG_CONTAINER_DETAIL,"container_tree_map_init");
	tmap->key_size     = key_size;
	tmap->data_size    = key_size + value_size;
	tmap->key_cmp_func = key_cmp_func;

	return ct->c_ops_p->init(ct,tmap->data_size);
}
static inline int container_push_front(container_t *ct,void *data)
{
	return ct->c_ops_p->push_front(ct,data);
}
static inline int container_push_back(container_t *ct,void *data)
{
	return ct->c_ops_p->push_back(ct,data);
}
static inline int container_pop_front(container_t *ct)
{
	return ct->c_ops_p->pop_front(ct);
}
static inline int container_pop_back(container_t *ct)
{
	return ct->c_ops_p->pop_back(ct);
}
static inline int container_map_insert(container_t *ct, pair_t *p)
{
	return ct->c_ops_p->map_insert(ct,p->data);
}
static inline int container_insert(container_t *ct,iterator_t *it, void *data)
{
	return ct->c_ops_p->insert(ct,it,data);
}
static inline int container_delete(container_t *ct, iterator_t *it)
{
	return ct->c_ops_p->del(ct,it);
}
static inline iterator_t *container_map_search(container_t *ct, void *key,iterator_t *it)
{
	return ct->c_ops_p->map_search(ct,key,it);
}
static inline int container_destroy(container_t **ct)
{
	(*ct)->c_ops_p->destroy(*ct);
	allocator_mem_free((*ct)->allocator,*ct);
	*ct = NULL;
	return 0;
}

static inline iterator_t * container_begin(container_t *ct,iterator_t *begin)
{
	return ct->c_ops_p->begin(ct,begin);
}
static inline iterator_t * container_end(container_t *ct,iterator_t *end)
{
	return ct->c_ops_p->end(ct,end);
}
static inline iterator_t * iterator_next(iterator_t *it,iterator_t *next)
{
	return it->container_p->it_ops_p->next(it,next);
}
static inline int iterator_prev(iterator_t *it,iterator_t *prev)
{
	return it->container_p->it_ops_p->prev(it,prev);
}
static inline int iterator_equal(iterator_t *it1,iterator_t *it2)
{
	return it1->container_p->it_ops_p->equal(it1,it2);
}
static inline void* iterator_get_pointer(iterator_t *it)
{
	return it->container_p->it_ops_p->iterator_get_pointer(it);
}
#endif
