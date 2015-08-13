#include <stdio.h>
#include "libdbg/debug.h"
#include "liballoc/allocator.h"

allocator_module_t allocator_modules[ALLOCATOR_TYPE_LAST];

allocator_t *allocator_creator(uint8_t allocator_type)
{
	allocator_t *p;
	p = (allocator_t *)malloc(sizeof(allocator_t));
	if(p == NULL){
		dbg_str(DBG_ERROR,"allocator_creator");
		return p;
	}
	p->allocator_type = allocator_type;

	return p;
}
void allocator_ctr_init(allocator_t * alloc,
		uint32_t slab_array_max_num, uint32_t data_min_size,
		uint32_t mempool_capacity)
{
	cds_alloc_t *alloc_p = &alloc->priv.cds_alloc;

	alloc_p->slab_array_max_num = slab_array_max_num;
	alloc_p->data_min_size = data_min_size;
	alloc_p->mempool_capacity = mempool_capacity;

	return allocator_modules[alloc->allocator_type].alloc_ops.init(alloc);

}
void *allocator_mem_alloc(allocator_t * alloc,uint32_t size)
{
	return allocator_modules[alloc->allocator_type].alloc_ops.alloc(alloc,size);
}

void allocator_mem_free(allocator_t * alloc,void *addr)
{
	allocator_modules[alloc->allocator_type].alloc_ops.free(alloc,addr);

}
void allocator_mem_info(allocator_t * alloc)
{
	if(allocator_modules[alloc->allocator_type].alloc_ops.info)
		allocator_modules[alloc->allocator_type].alloc_ops.info(alloc);
}
void allocator_destroy(allocator_t * alloc)
{
	uint8_t allocator_type = alloc->allocator_type;
	dbg_str(DBG_WARNNING,"allocator_destroy");
	if(allocator_modules[allocator_type].alloc_ops.destroy){
		allocator_modules[allocator_type].alloc_ops.destroy(alloc);
	}
	free(alloc);
}
