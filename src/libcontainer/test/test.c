#include <stdio.h>
#include <string.h>
#include "libdbg/debug.h"
#include "libcontainer/basic_types.h"
#include "libcontainer/test_container.h"

struct test test={
	.name = "abc",
	.len = 3
};
struct test test2={
	.name = "bca",
	.len = 4
};
struct test test3={
	.name = "cbc",
	.len =8
};
struct test test4={
	.name = "dbc",
	.len = 6
};
void print_test(struct test *t)
{
	dbg_str(DBG_CONTAINER_DETAIL,"name=%s len=%d",t->name,t->len);
}
uint32_t test_hash_func(void *key,uint32_t key_size)
{
	uint16_t num = *(uint16_t *)key;
	dbg_buf(DBG_CONTAINER_DETAIL,"test hash func key:",key,key_size);
	return num % 10;
}
uint32_t test_key_cmp_func(void *key1,void *key2,uint32_t size)
{
	dbg_buf(DBG_CONTAINER_DETAIL,"cmp key1:",key1,size);
	dbg_buf(DBG_CONTAINER_DETAIL,"cmp key2:",key2,size);
	return memcmp(key1,key2,size);
}

