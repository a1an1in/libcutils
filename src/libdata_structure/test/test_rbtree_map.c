#include <stdio.h>
#include <libdata_structure/rbtree_map.h>
#include <libdbg/debug.h>

struct A{
    int a;
    int b;
};
void print_A(struct A *t)
{
    dbg_str(DBG_DETAIL,"a=%d b=%d",t->a,t->b);
}
#if 0
int test_datastructure_rbtree_map(void)
{
    int ret = 0;
    rbtree_map_pos_t it,next,end;
    rbtree_map_t *map;
    allocator_t *allocator = allocator_get_default_alloc();
    pair_t *pair;
    struct rbtree_map_node *mnode;
    int key_len = 2;

    struct A t1 = {1,2};
    struct A t2 = {2,2};
    struct A t3 = {3,2};
    struct A t4 = {4,2};
    struct A t5 = {5,2};

    dbg_str(DBG_DETAIL,"rbtree allocator addr:%p",allocator);
    map = rbtree_map_create(allocator,0);

    rbtree_map_init(map,key_len,sizeof(struct A) + key_len,NULL); 

    dbg_str(DBG_DETAIL,"create_pair");
    pair = create_pair(key_len,sizeof(struct A));

    make_pair(pair,"44",&t4);
    rbtree_map_insert(map,pair->data);
    make_pair(pair,"33",&t3);
    rbtree_map_insert(map,pair->data);
    make_pair(pair,"22",&t2);
    rbtree_map_insert(map,pair->data);
    make_pair(pair,"11",&t1);
    rbtree_map_insert(map,pair->data);


    dbg_str(DBG_DETAIL,"foreach ordinal print");
    for(    rbtree_map_begin(map,&it),rbtree_map_pos_next(&it,&next); 
            !rbtree_map_pos_equal(&it,rbtree_map_end(map,&end));
            it = next,rbtree_map_pos_next(&it,&next))
    {
        mnode = rb_entry(it.rb_node_p,struct rbtree_map_node,node);
        dbg_buf(DBG_DETAIL,"key:",mnode->key,mnode->value_pos);
        print_A((struct A*)rbtree_map_pos_get_pointer(&it));
    }

    dbg_str(DBG_DETAIL,"search node key = 22");
    rbtree_map_search(map, (void *)"22",&it);
    print_A((struct A*)rbtree_map_pos_get_pointer(&it));

    /*
     *dbg_str(DBG_DETAIL,"delte node key = 11");
     *container_delete(map,&it);
     */

    rbtree_map_destroy(map);
    allocator_destroy(allocator);
    dbg_str(DBG_DETAIL,"map addr =%p",map);

    return ret;
}
#else
int test_datastructure_rbtree_map(void)
{
    int ret = 0;
    rbtree_map_pos_t it,next,end;
    rbtree_map_t *map;
    allocator_t *allocator = allocator_get_default_alloc();
    pair_t *pair;
    struct rbtree_map_node *mnode;
    int key_len = 2;

    struct A t1 = {1,2};
    struct A t2 = {2,2};
    struct A t3 = {3,2};
    struct A t4 = {4,2};
    struct A t5 = {5,2};

    dbg_str(DBG_DETAIL,"rbtree allocator addr:%p",allocator);
    map = rbtree_map_alloc(allocator);

    rbtree_map_init(map,key_len,sizeof(struct A)); 


    rbtree_map_insert(map,(char *)"44",&t4);
    rbtree_map_insert(map,(char *)"33",&t3);
    rbtree_map_insert(map,(char *)"22",&t2);
    rbtree_map_insert(map,(char *)"11",&t1);


    dbg_str(DBG_DETAIL,"foreach ordinal print");
    for(    rbtree_map_begin(map,&it),rbtree_map_pos_next(&it,&next); 
            !rbtree_map_pos_equal(&it,rbtree_map_end(map,&end));
            it = next,rbtree_map_pos_next(&it,&next))
    {
        mnode = rb_entry(it.rb_node_p,struct rbtree_map_node,node);
        dbg_buf(DBG_DETAIL,"key:",mnode->key,mnode->value_pos);
        print_A((struct A*)rbtree_map_pos_get_pointer(&it));
    }

    dbg_str(DBG_DETAIL,"search node key = 22");
    rbtree_map_search(map, (void *)"22",&it);
    print_A((struct A*)rbtree_map_pos_get_pointer(&it));

    /*
     *dbg_str(DBG_DETAIL,"delte node key = 11");
     *container_delete(map,&it);
     */

    rbtree_map_destroy(map);
    allocator_destroy(allocator);
    dbg_str(DBG_DETAIL,"map addr =%p",map);

    return ret;
}
#endif
