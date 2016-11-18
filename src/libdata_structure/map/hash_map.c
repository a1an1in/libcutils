#include <libdata_structure/map.h>


/*
 *map_t * map_create(allocator_t *allocator,uint8_t lock_type);
 *int map_insert_data(map_t *hmap,void *data);
 *int map_insert_data_wb(map_t *hmap,void *data, map_pos_t *out);
 *int map_insert(map_t *hmap,void *key,void *value);
 *int map_insert_wb(map_t *hmap,void *key,void *value, map_pos_t *out);
 *int map_search(map_t *hmap, void *key,map_pos_t *ret);
 *int map_delete(map_t *hmap, map_pos_t *pos);
 *int map_destroy(map_t *hmap);
 *int map_init(map_t *hmap, uint32_t key_size, uint32_t data_size, uint32_t bucket_size, hash_func_fpt hash_func, key_cmp_fpt key_cmp_func);
 *int map_pos_next(map_pos_t *pos,map_pos_t *next);
 *void map_print_mnode(struct map_node *mnode);
 */

/*
 *map_module_t map_modules[MAP_TYPE_MAX_NUM];
 */
