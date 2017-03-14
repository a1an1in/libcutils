#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__
#include "liballoc/allocator_ctr_alloc.h"

enum allocator_type{
	ALLOCATOR_TYPE_SYS_MALLOC = 0,
	ALLOCATOR_TYPE_CTR_MALLOC,
	ALLOCATOR_TYPE_LAST
};
typedef struct allocator{
#	define COTAINOR_NAME_MAX_LEN 40
	char name[COTAINOR_NAME_MAX_LEN];
	uint8_t allocator_type;
	uint8_t lock_type;
	union{
		ctr_alloc_t ctr_alloc;
	}priv;
	struct allocator_operations *alloc_ops;
	uint32_t alloc_count;
#	undef COTAINOR_NAME_MAX_LEN
}allocator_t;

struct allocator_operations{
	int (*init)(allocator_t *alloc);
	void *(*alloc)(allocator_t *alloc,uint32_t size);
	void (*free)(allocator_t *alloc,void *addr);
	void (*destroy)(allocator_t * alloc);
	void (*info)(allocator_t * alloc);
	void (*tag)(allocator_t *alloc,void *addr, void *tag);
};
typedef struct allocator_module{
	uint8_t allocator_type;
	struct allocator_operations alloc_ops;
}allocator_module_t;

extern allocator_module_t allocator_modules[ALLOCATOR_TYPE_LAST];
allocator_t *allocator_create(uint8_t allocator_type,uint8_t lock_type);
void allocator_destroy(allocator_t * alloc);
void allocator_ctr_init(allocator_t * alloc, uint32_t slab_array_max_num, uint32_t data_min_size, uint32_t mempool_capacity);

static inline void *
allocator_mem_alloc(allocator_t * alloc,uint32_t size)
{
	return allocator_modules[alloc->allocator_type].alloc_ops.alloc(alloc,size);
}

static inline void 
allocator_mem_free(allocator_t * alloc,void *addr)
{
	allocator_modules[alloc->allocator_type].alloc_ops.free(alloc,addr);
}

static inline void 
allocator_mem_tag(allocator_t * alloc,void *addr, void *tag)
{
    if(allocator_modules[alloc->allocator_type].alloc_ops.tag) {
        allocator_modules[alloc->allocator_type].alloc_ops.tag(alloc,addr, tag);
    }
}

static inline void 
allocator_mem_info(allocator_t * alloc)
{
    if(allocator_modules[alloc->allocator_type].alloc_ops.info)
        allocator_modules[alloc->allocator_type].alloc_ops.info(alloc);
}
allocator_t * allocator_get_default_alloc();
#endif
