#include <string.h>
#include <stdlib.h>

#include "libdbg/debug.h"

void *mem_alloc_pck(void *from,uint32_t size,uint32_t flag)
{
	void *ret = NULL;

	if(from == NULL){
		ret =  malloc(size);
		if(ret != NULL)
			memset(ret,0,size);
		/*
		 *else
		 *    dbg_str(DBG_ERROR,"malloc err");
		 */
	}
	return ret;
}
