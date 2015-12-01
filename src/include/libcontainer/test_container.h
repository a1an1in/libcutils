/*
 * =====================================================================================
 *
 *       Filename:  test.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/12/2015 05:35:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __TEST_H__
#define __TEST_H__
struct test{
	char name[10];
	char len;
};
extern struct test test,test2,test3,test4;
int test_hash_map(void);
int test_list(void);
int test_container_rbtree_map(void);
int test_vector(void);
void print_test(struct test *t);
void test_cds_alloc();
uint32_t test_key_cmp_func(void *key1,void *key2,uint32_t size);
uint32_t test_hash_func(void *key,uint32_t key_size);
#endif
