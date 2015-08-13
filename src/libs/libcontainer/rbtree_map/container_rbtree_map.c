/*
 * =====================================================================================
 *
 *       Filename:  container_rbtree_map.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "libcontainer/container_rbtree_map.h"
#include "libcontainer/inc_files.h"

iterator_t rbtree_map_iterator_init(iterator_t *it, struct rb_node *node,container_t *ct);
int rbtree_map_iterator_equal(iterator_t it1,iterator_t it2);


static int default_key_cmp_func(void *key1,void *key2,uint32_t size)
{
	return memcmp(key1,key2,size);
}

struct rbtree_map_node * __rbtree_map_search(container_t *ct,struct rb_root *root, void *key)
{
  	struct rb_node *node = root->rb_node;
	key_cmp_fpt key_cmp_func = ct->priv.rbtree_map_priv.key_cmp_func;

  	while (node) {
  		struct rbtree_map_node *mnode = rb_entry(node, struct rbtree_map_node, node);
		int result;

		//result = strcmp(key, mnode->key);
		result = key_cmp_func(key, mnode->key,mnode->value_pos);

		if (result < 0)
  			node = node->rb_left;
		else if (result > 0)
  			node = node->rb_right;
		else
  			return mnode;
	}
	return NULL;
}

int __rbtree_map_insert(container_t *ct,struct rb_root *root, struct rbtree_map_node *new_mnode)
{
  	struct rb_node **new = &(root->rb_node), *parent = NULL;
	key_cmp_fpt key_cmp_func = ct->priv.rbtree_map_priv.key_cmp_func;

  	/* Figure out where to put new node */
  	while (*new) {
  		struct rbtree_map_node *this = rb_entry(*new, struct rbtree_map_node, node);
  		//int result = strcmp(new_mnode->key, this->key);
		int result;
		result = key_cmp_func(new_mnode->key, this->key,this->value_pos);

		parent = *new;
  		if (result < 0)
  			new = &((*new)->rb_left);
  		else if (result > 0)
  			new = &((*new)->rb_right);
  		else
  			return 0;
  	}

  	/* Add new node and rebalance tree. */
  	rb_link_node(&new_mnode->node, parent, new);
  	rb_insert_color(&new_mnode->node, root);

	return 1;
}
/*
 *int rbtree_map_init(container_t *ct, uint32_t key_size,
 *        uint32_t value_size, key_cmp_fpt key_cmp_func)
 *{
 *    struct rb_root *tree_root;
 *
 *    dbg_str(DBG_CONTAINER_DETAIL,"rbtree_map init");
 *
 *    strcpy(ct->name,"rbtree_map container");
 *
 *    if(key_cmp_func == NULL){
 *        ct->priv.rbtree_map_priv.key_cmp_func = default_key_cmp_func;
 *    }else{
 *        ct->priv.rbtree_map_priv.key_cmp_func = key_cmp_func;
 *    }
 *    ct->priv.rbtree_map_priv.key_size = key_size;
 *    ct->priv.rbtree_map_priv.data_size = key_size + value_size;
 *
 *    tree_root = (struct rb_root *)allocator_mem_alloc(ct->allocator,sizeof(struct rb_root));
 *    tree_root->rb_node = NULL;
 *    ct->priv.rbtree_map_priv.tree_root = tree_root;
 *    rbtree_map_iterator_init(&ct->end,NULL,ct);
 *    rbtree_map_iterator_init(&ct->begin,NULL,ct);
 *
 *    return 0;
 *}
 */
int rbtree_map_init(container_t *ct)
{
	struct rb_root *tree_root;
	key_cmp_fpt key_cmp_func = ct->priv.rbtree_map_priv.key_cmp_func;
	dbg_str(DBG_CONTAINER_DETAIL,"rbtree_map init");

	strcpy(ct->name,"rbtree_map container");

	if(key_cmp_func == NULL){
		ct->priv.rbtree_map_priv.key_cmp_func = default_key_cmp_func;
	}

	tree_root = (struct rb_root *)allocator_mem_alloc(ct->allocator,sizeof(struct rb_root));
	tree_root->rb_node = NULL;
	ct->priv.rbtree_map_priv.tree_root = tree_root;
	rbtree_map_iterator_init(&ct->end,NULL,ct);
	rbtree_map_iterator_init(&ct->begin,NULL,ct);

	return 0;
}

iterator_t rbtree_map_begin(container_t *ct)
{
	return ct->begin;
}

iterator_t rbtree_map_end(container_t *ct)
{
	return ct->end;
}
int rbtree_map_insert(container_t *ct, void *value)
{
	struct rbtree_map_node *mnode;
	struct rb_root *tree_root = ct->priv.rbtree_map_priv.tree_root;
	uint32_t data_size = ct->priv.rbtree_map_priv.data_size;
	uint32_t key_size = ct->priv.rbtree_map_priv.key_size;

	dbg_str(DBG_CONTAINER_DETAIL,"rbtree_map_insert");
	mnode = (struct rbtree_map_node *)allocator_mem_alloc(ct->allocator,sizeof(struct rbtree_map_node) + data_size);
	if(mnode == NULL){
		dbg_str(DBG_CONTAINER_ERROR,"rbtree_map_insert,malloc err");
		return -1;
	}

	memcpy(mnode->key,value,data_size);
	mnode->value_pos = key_size;

	pthread_rwlock_wrlock(&ct->head_lock);
	__rbtree_map_insert(ct,tree_root, mnode);

	ct->begin.pos.rb_node_p = rb_first(tree_root);
	pthread_rwlock_unlock(&ct->head_lock);

	return 0;
}
int rbtree_map_delete(container_t *ct, iterator_t it)
{
	struct rbtree_map_node *mnode;
	struct rb_node *rb_node_p = it.pos.rb_node_p;
	struct rb_root *tree_root = ct->priv.rbtree_map_priv.tree_root;

	mnode = rb_entry(rb_node_p,struct rbtree_map_node,node);

	dbg_str(DBG_CONTAINER_DETAIL,"delete node");

	pthread_rwlock_wrlock(&ct->head_lock);
	if(rbtree_map_iterator_equal(it,ct->begin)){
		rbtree_map_iterator_init(&ct->begin,rb_next(rb_node_p),ct);
	}
	rb_erase(rb_node_p, tree_root);
	pthread_rwlock_unlock(&ct->head_lock);

	if (mnode != NULL) {
		allocator_mem_free(ct->allocator,mnode);
		mnode = NULL;
	}
	return 0;
}
iterator_t rbtree_map_search(container_t *ct, void *key)
{
	iterator_t ret;
	struct rbtree_map_node *mnode;
	struct rb_root *tree_root = ct->priv.rbtree_map_priv.tree_root;

	dbg_str(DBG_CONTAINER_DETAIL,"rbtree_map_search");

	pthread_rwlock_rdlock(&ct->head_lock);
	mnode = __rbtree_map_search(ct,tree_root, key);
	pthread_rwlock_unlock(&ct->head_lock);

	if(mnode == NULL){
		ret.pos.rb_node_p = NULL;
	}
	ret.pos.rb_node_p = &mnode->node;
	ret.container_p = ct;

	return ret;
}
int rbtree_map_destroy(container_t *ct)
{
	iterator_t it;
	struct rb_root *tree_root = ct->priv.rbtree_map_priv.tree_root;

	dbg_str(DBG_CONTAINER_DETAIL,"rbtree_map_destroy");
	for(	it = rbtree_map_begin(ct); 
			!rbtree_map_iterator_equal(it,container_end(ct));
			it = rbtree_map_begin(ct)) 
	{
		rbtree_map_delete(ct,it);
	}
	if(rbtree_map_iterator_equal(ct->end,ct->begin)){
		dbg_str(DBG_CONTAINER_WARNNING,"rbtree_map_destroy,rbtree_map is NULL");
		dbg_str(DBG_CONTAINER_WARNNING,"release tree_root");
		allocator_mem_free(ct->allocator,tree_root);
	}
}

iterator_t rbtree_map_iterator_init(iterator_t *it,
		struct rb_node *node,
		container_t *ct)
{
	it->container_p = ct;
	it->pos.rb_node_p = node;

	return *it;
}
iterator_t rbtree_map_iterator_next(iterator_t it)
{
	return rbtree_map_iterator_init(&it,
			rb_next(it.pos.rb_node_p),
			it.container_p);
}
iterator_t rbtree_map_iterator_prev(iterator_t it)
{
	return rbtree_map_iterator_init(&it,
			rb_prev(it.pos.rb_node_p),
			it.container_p);
}
int rbtree_map_iterator_equal(iterator_t it1,iterator_t it2)
{
	return it1.pos.rb_node_p == it2.pos.rb_node_p;
}
void *rbtree_map_iterator_get_pointer(iterator_t it)
{
	struct rbtree_map_node *mnode;

	mnode = rb_entry(it.pos.rb_node_p,struct rbtree_map_node,node);

	return &mnode->key[mnode->value_pos];
}

int container_rbtree_map_register(){
	container_module_t cm = {
		.name = "rbtree_map",
		.container_type = CONTAINER_TYPE_RBTREE_MAP,
		.c_ops = {
			.init   = rbtree_map_init,
			.begin      = rbtree_map_begin,
			.end        = rbtree_map_end,
			.map_search = rbtree_map_search,
			.del        = rbtree_map_delete,
			.map_insert = rbtree_map_insert,
			.destroy    = rbtree_map_destroy,
		},
		.it_ops = {
			.next = rbtree_map_iterator_next,
			.prev = rbtree_map_iterator_prev,
			.equal = rbtree_map_iterator_equal,
			.iterator_get_pointer = rbtree_map_iterator_get_pointer
		}
	};
	memcpy(&container_modules[CONTAINER_TYPE_RBTREE_MAP],&cm,sizeof(container_module_t));
	return 0;
}


