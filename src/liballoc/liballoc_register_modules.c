#include <stdio.h>

void liballoc_register_modules()
{
	allocator_sys_alloc_register();
	allocator_cds_alloc_register();
}

