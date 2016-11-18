#ifndef __MAP_H__
#define __MAP_H__

#include <libdata_structure/map_struct.h>

static inline int map_set(map_t *map, char *attrib, char *value)
{
    return map->map_ops->map_set(map,attrib,value);
}

static inline int map_init(map_t *map, uint32_t key_size, uint32_t value_size)
{
    return map->map_ops->map_init(map,key_size,value_size);
}

static inline int map_insert(map_t *map, char *key, void *value)
{
    return map->map_ops->map_insert(map, key, value);
}

static inline int map_search(map_t *map, void *key,map_iterator_t *it)
{
    return map->map_ops->map_search(map,key,it);
}

static inline int map_del(map_t *map, map_iterator_t *it)
{
    return map->map_ops->map_del(map,it);
}

static inline int map_destroy(map_t *map)
{
    return map->map_ops->map_destroy(map);
}

static inline int map_begin(map_t *map, map_iterator_t *it)
{
    map->map_ops->map_begin(map, it);
    it->map = map;

    return 0;
}

static inline int map_end(map_t *map, map_iterator_t *it)
{
    map->map_ops->map_end(map, it);
    it->map = map;

    return 0;
}

static inline int  map_next(map_iterator_t *it, map_iterator_t *next)
{
    it->map->it_ops->map_next(it,next);
    next->map = it->map;

    return 0;
}

static inline int map_prev(map_iterator_t *it, map_iterator_t *prev)
{
    it->map->it_ops->map_prev(it,prev);
    prev->map = it->map;

    return 0;
}

static inline int map_equal(map_iterator_t *it1,map_iterator_t *it2)
{
    return it1->map->it_ops->map_equal(it1,it2);
}

static inline void * map_get_pointer(map_iterator_t *it)
{
    return it->map->it_ops->map_get_pointer(it);
}

static inline void 
map_for_each(map_t *map,void (*func)(map_iterator_t *it))
{
	map_iterator_t it,next,end;

    map_end(map, &end);
	for(	map_begin(map,&it),map_next(&it,&next); 
			!map_equal(&it,&end);
			it = next,map_next(&it,&next)){
		func(&it);
	}
}

#endif
