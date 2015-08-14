#include <stdio.h>
#include "liballoc/inc_files.h"

void *sys_alloc_alloc(allocator_t *alloc,uint8_t size)
{
	return malloc(size);
}
void sys_alloc_free(allocator_t *alloc,void *addr)
{
	free(addr);
}
int allocator_sys_alloc_register(){
	allocator_module_t salloc = {
		.allocator_type = ALLOCATOR_TYPE_SYS_MALLOC,
		.alloc_ops = {
			.init    = NULL,
			.alloc   = sys_alloc_alloc,
			.free    = sys_alloc_free,
			.destroy = NULL,
		}
	};
	memcpy(&allocator_modules[ALLOCATOR_TYPE_SYS_MALLOC],&salloc,sizeof(allocator_module_t));
	return 0;
}
