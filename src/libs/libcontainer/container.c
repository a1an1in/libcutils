/*
 * =====================================================================================
 *
 *       Filename:  container.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/1/2015 
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "libcontainer/inc_files.h"

container_module_t container_modules[CONTAINER_TYPE_MAX_NUM];

pair_t * create_pair(int key_len,int value_len)
{
	pair_t *p;
	p = (pair_t *)malloc(sizeof(pair_t) + key_len + value_len);
	p->key_len = key_len;
	p->value_len = value_len;
	p->data_len = key_len + value_len;
	return p;
}
void make_pair(pair_t *p,void *key,void *value)
{
	memcpy(p->data,key,p->key_len);
	memcpy(p->data + p->key_len,value,p->value_len);
	dbg_buf(DBG_CONTAINER_DETAIL,"make pair:",p->data,p->data_len);
}
int destroy_pair(pair_t *p)
{
	free(p);
	p = NULL;
	return 0;
}

container_t *container_creator(uint32_t container_type,allocator_t *allocator)
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
	pthread_rwlock_init(&p->head_lock,NULL);   

	if(p->c_ops_p == NULL || p->it_ops_p == NULL){
		allocator_mem_free(p->allocator,p);
		p = NULL;
	}
err:
	return p;
}
int container_vector_init(container_t *ct,uint32_t data_size,uint32_t capacity)
{
	ct->priv.vector_priv.data_size = data_size;
	ct->priv.vector_priv.capacity  = capacity;
	dbg_str(DBG_CONTAINER_DETAIL,"container_vector_init");

	return ct->c_ops_p->init(ct);
}
int container_list_init(container_t *ct,uint32_t data_size)
{
	ct->priv.list_priv.data_size = data_size;

	return ct->c_ops_p->init(ct);
}
/*
 *int container_map_init(container_t *ct,
 *        uint32_t key_size, uint32_t value_size,
 *        key_cmp_fpt key_cmp_func)
 *{
 *    return ct->c_ops_p->map_init(ct,key_size,value_size,key_cmp_func);
 *}
 */
int container_tree_map_init(container_t *ct,
		uint32_t key_size, uint32_t value_size,
		key_cmp_fpt key_cmp_func)
{
	ct->priv.rbtree_map_priv.key_size = key_size;
	ct->priv.rbtree_map_priv.data_size = key_size + value_size;
	ct->priv.rbtree_map_priv.key_cmp_func = key_cmp_func;
	return ct->c_ops_p->init(ct);
}
int container_hash_map_init(container_t *ct, uint32_t key_size,
		uint32_t value_size, uint32_t bucket_size,
		hash_func_fpt hash_func, key_cmp_fpt key_cmp_func)
{
	hash_map_priv_t *hpi = &ct->priv.hash_map_priv;

	dbg_str(DBG_CONTAINER_DETAIL,"container_hash_map_init");
	hpi->key_size    = key_size;
	hpi->data_size   = key_size + value_size;
	hpi->hash_func   = hash_func;
	hpi->key_cmp_fpt = key_cmp_func;
	hpi->bucket_size = bucket_size;
	dbg_str(DBG_CONTAINER_DETAIL,"ct->c_ops_p->init(ct)");

	return ct->c_ops_p->init(ct);
}
int container_push_front(container_t *ct,void *data)
{
	return ct->c_ops_p->push_front(ct,data);
}
int container_push_back(container_t *ct,void *data)
{
	return ct->c_ops_p->push_back(ct,data);
}
int container_pop_front(container_t *ct)
{
	return ct->c_ops_p->pop_front(ct);
}
int container_pop_back(container_t *ct)
{
	return ct->c_ops_p->pop_back(ct);
}
iterator_t container_begin(container_t *ct)
{
	dbg_str(DBG_CONTAINER_DETAIL,"container_begin");
	ct->c_ops_p->begin(ct);
	return ct->c_ops_p->begin(ct);
}
iterator_t container_end(container_t *ct)
{
	return ct->c_ops_p->end(ct);
}
int container_map_insert(container_t *ct, pair_t *p)
{
	return ct->c_ops_p->map_insert(ct,p->data);
}
int container_insert(container_t *ct,iterator_t it, void *data)
{
	return ct->c_ops_p->insert(ct,it,data);
}
int container_delete(container_t *ct, iterator_t it)
{
	return ct->c_ops_p->del(ct,it);
}
iterator_t container_map_search(container_t *ct, void *key)
{
	return ct->c_ops_p->map_search(ct,key);
}
int container_destroy(container_t **ct)
{
	(*ct)->c_ops_p->destroy(*ct);
	allocator_mem_free((*ct)->allocator,*ct);
	*ct = NULL;
	return 0;
}

iterator_t iterator_next(iterator_t it)
{
	return it.container_p->it_ops_p->next(it);
}
iterator_t iterator_prev(iterator_t it)
{
	return it.container_p->it_ops_p->prev(it);
}
int iterator_equal(iterator_t it1,iterator_t it2)
{
	return it1.container_p->it_ops_p->equal(it1,it2);
}
void* iterator_get_pointer(iterator_t it)
{
	return it.container_p->it_ops_p->iterator_get_pointer(it);
}
