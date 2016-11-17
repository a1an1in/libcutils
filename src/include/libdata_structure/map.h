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

typedef struct iterator{
    union{
        struct rb_node *rb_node_p;
        hash_map_pos_t hash_map_pos;
    }pos;
     struct map *map_p;
}iterator_t;


typedef struct map{
#	define MAP_NAME_MAX_LEN 40
    char name[MAP_NAME_MAX_LEN];
    uint8_t map_type;
    allocator_t *allocator;
    struct map_operations *c_ops_p;
    struct iterator_operations *it_ops_p;
    struct iterator begin,end,cur;
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
    int (*push_front)(map_t *ct,void *data);
    int (*push_back)(map_t *ct,void *data);
    int (*pop_front)(map_t *ct);
    int (*pop_back)(map_t *ct);
    iterator_t (*begin)(map_t *ct);
    iterator_t (*end)(map_t *ct);
    int (*insert)(map_t *ct, iterator_t it,void *value);
    int (*map_insert)(map_t *ct,void *value);
    iterator_t (*map_search)(map_t *ct, void *key);
    int (*del)(map_t *ct, iterator_t it);
    int (*destroy)(map_t *ct);
};

struct iterator_operations{
    iterator_t (*next)(iterator_t it);
    iterator_t (*prev)(iterator_t it);
    int (*equal)(iterator_t it1,iterator_t it2);
    void *(*iterator_get_pointer)(iterator_t it);
};

typedef struct map_module{
#	define MAP_NAME_MAX_LEN 20
    char name[MAP_NAME_MAX_LEN];
    uint8_t map_type;
    struct map_operations c_ops;
    struct iterator_operations it_ops;
    struct iterator begin,end;
#	undef MAP_NAME_MAX_LEN
}map_module_t;

extern map_module_t map_modules[MAP_TYPE_MAX_NUM];

#endif
