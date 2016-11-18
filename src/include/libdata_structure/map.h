#ifndef __MAP_H__
#define __MAP_H__

#include <basic_types.h>
#include <libdata_structure/hash_list.h>
#include <libdata_structure/rbtree_map.h>

enum map_type{
    MAP_TYPE_RBTREE_MAP,
    MAP_TYPE_HASH_MAP,
    MAP_TYPE_MAX_NUM
};

typedef struct map_interator{
    union{
        rbtree_map_pos_t rbtree_map_pos;
        hash_map_pos_t hash_map_pos;
    }pos;
}map_interator_t;


typedef struct map{
#	define MAP_NAME_MAX_LEN 40
    char name[MAP_NAME_MAX_LEN];
    uint8_t map_type;
    allocator_t *allocator;
    struct map_operations *c_ops_p;
    struct map_interator_operations *it_ops_p;
    struct map_interator begin,end,cur;
    pthread_rwlock_t head_lock;    
    union{
        rbtree_map_t *rbtree_map;
        hash_map_t *hash_map;
    }priv;
#	undef MAP_NAME_MAX_LEN
}map_t;

struct map_operations{
    int (*init)(map_t *);
    int (*map_init)(map_t *ct, uint32_t key_size, uint32_t value_size, key_cmp_fpt key_cmp_func);
    map_interator_t (*begin)(map_t *ct);
    map_interator_t (*end)(map_t *ct);
    int (*map_insert)(map_t *ct,void *value);
    map_interator_t (*map_search)(map_t *ct, void *key);
    int (*del)(map_t *ct, map_interator_t it);
    int (*destroy)(map_t *ct);
};

struct map_interator_operations{
    map_interator_t (*next)(map_interator_t it);
    map_interator_t (*prev)(map_interator_t it);
    int (*equal)(map_interator_t it1,map_interator_t it2);
    void *(*map_interator_get_pointer)(map_interator_t it);
};

typedef struct map_module{
#	define MAP_NAME_MAX_LEN 20
    char name[MAP_NAME_MAX_LEN];
    uint8_t map_type;
    struct map_operations c_ops;
    struct map_interator_operations it_ops;
    struct map_interator begin,end;
#	undef MAP_NAME_MAX_LEN
}map_module_t;

extern map_module_t map_modules[MAP_TYPE_MAX_NUM];

#endif
