/*
 * =====================================================================================
 *
 *       Filename:  debug_businesses.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/25/2015 06:02:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __DEBUG_BUSINESSES_H__
#define __DEBUG_BUSINESSES_H__
enum debug_business_enum{
	DBG_BUSINESS_NORMAL = 0,
	DBG_BUSINESS_ALLOC,
	DBG_BUSINESS_CONTAINER,
	DBG_BUSINESS_LINKLIST,
	DBG_BUSINESS_HASHMAP,
	DBG_BUSINESS_CONCURRENT,
	DBG_BUSINESS_NET,
	DBG_BUSINESS_PA,
	DBG_BUSINESS_ARGS,
	MAX_DEBUG_BUSINESS_NUM
};
char *debug_business_names[MAX_DEBUG_BUSINESS_NUM];

#define ALLOC_DBG_PANIC			 DBG_BUSINESS_ALLOC 	 << 8 | DBG_DBG_PANIC 
#define ALLOC_FATAL				 DBG_BUSINESS_ALLOC 	 << 8 | DBG_FATAL 
#define ALLOC_ERROR				 DBG_BUSINESS_ALLOC 	 << 8 | DBG_ERROR 
#define ALLOC_WARNNING			 DBG_BUSINESS_ALLOC 	 << 8 | DBG_WARNNING 
#define ALLOC_SUC				 DBG_BUSINESS_ALLOC 	 << 8 | DBG_SUC 
#define ALLOC_CORRECT			 DBG_BUSINESS_ALLOC 	 << 8 | DBG_CORRECT 
#define ALLOC_VIP 				 DBG_BUSINESS_ALLOC 	 << 8 | DBG_VIP 
#define ALLOC_FLOW				 DBG_BUSINESS_ALLOC 	 << 8 | DBG_FLOW 
#define ALLOC_IMPORTANT			 DBG_BUSINESS_ALLOC 	 << 8 | DBG_IMPORTANT 
#define ALLOC_DETAIL			 DBG_BUSINESS_ALLOC 	 << 8 | DBG_DETAIL 

#define LINKLIST_PANIC		 	 DBG_BUSINESS_LINKLIST 	 << 8 | DBG_DBG_PANIC 
#define LINKLIST_FATAL			 DBG_BUSINESS_LINKLIST 	 << 8 | DBG_FATAL 
#define LINKLIST_ERROR			 DBG_BUSINESS_LINKLIST 	 << 8 | DBG_ERROR 
#define LINKLIST_WARNNING		 DBG_BUSINESS_LINKLIST 	 << 8 | DBG_WARNNING 
#define LINKLIST_SUC			 DBG_BUSINESS_LINKLIST 	 << 8 | DBG_SUC 
#define LINKLIST_CORRECT		 DBG_BUSINESS_LINKLIST 	 << 8 | DBG_CORRECT 
#define LINKLIST_VIP 		  	 DBG_BUSINESS_LINKLIST 	 << 8 | DBG_VIP 
#define LINKLIST_FLOW			 DBG_BUSINESS_LINKLIST 	 << 8 | DBG_FLOW 
#define LINKLIST_IMPORTANT		 DBG_BUSINESS_LINKLIST 	 << 8 | DBG_IMPORTANT 
#define LINKLIST_DETAIL			 DBG_BUSINESS_LINKLIST 	 << 8 | DBG_DETAIL 

#define HMAP_PANIC		 		 DBG_BUSINESS_HASHMAP 	 << 8 | DBG_DBG_PANIC 
#define HMAP_FATAL				 DBG_BUSINESS_HASHMAP 	 << 8 | DBG_FATAL 
#define HMAP_ERROR				 DBG_BUSINESS_HASHMAP 	 << 8 | DBG_ERROR 
#define HMAP_WARNNING			 DBG_BUSINESS_HASHMAP 	 << 8 | DBG_WARNNING 
#define HMAP_SUC				 DBG_BUSINESS_HASHMAP 	 << 8 | DBG_SUC 
#define HMAP_CORRECT			 DBG_BUSINESS_HASHMAP 	 << 8 | DBG_CORRECT 
#define HMAP_VIP 		  		 DBG_BUSINESS_HASHMAP 	 << 8 | DBG_VIP 
#define HMAP_FLOW				 DBG_BUSINESS_HASHMAP 	 << 8 | DBG_FLOW 
#define HMAP_IMPORTANT			 DBG_BUSINESS_HASHMAP 	 << 8 | DBG_IMPORTANT 
#define HMAP_DETAIL		 		 DBG_BUSINESS_HASHMAP 	 << 8 | DBG_DETAIL 

#define CONCURRENT_PANIC		 DBG_BUSINESS_CONCURRENT << 8 | DBG_DBG_PANIC 
#define CONCURRENT_FATAL		 DBG_BUSINESS_CONCURRENT << 8 | DBG_FATAL 
#define CONCURRENT_ERROR		 DBG_BUSINESS_CONCURRENT << 8 | DBG_ERROR 
#define CONCURRENT_WARNNING		 DBG_BUSINESS_CONCURRENT << 8 | DBG_WARNNING 
#define CONCURRENT_SUC			 DBG_BUSINESS_CONCURRENT << 8 | DBG_SUC 
#define CONCURRENT_CORRECT		 DBG_BUSINESS_CONCURRENT << 8 | DBG_CORRECT 
#define CONCURRENT_VIP 		  	 DBG_BUSINESS_CONCURRENT << 8 | DBG_VIP 
#define CONCURRENT_FLOW			 DBG_BUSINESS_CONCURRENT << 8 | DBG_FLOW 
#define CONCURRENT_IMPORTANT	 DBG_BUSINESS_CONCURRENT << 8 | DBG_IMPORTANT 
#define CONCURRENT_DETAIL		 DBG_BUSINESS_CONCURRENT << 8 | DBG_DETAIL 

#define NET_PANIC				 DBG_BUSINESS_NET		 << 8 | DBG_DBG_PANIC 
#define NET_FATAL				 DBG_BUSINESS_NET		 << 8 | DBG_FATAL 
#define NET_ERROR				 DBG_BUSINESS_NET		 << 8 | DBG_ERROR 
#define NET_WARNNING			 DBG_BUSINESS_NET		 << 8 | DBG_WARNNING 
#define NET_SUC					 DBG_BUSINESS_NET		 << 8 | DBG_SUC 
#define NET_CORRECT				 DBG_BUSINESS_NET		 << 8 | DBG_CORRECT 
#define NET_VIP 				 DBG_BUSINESS_NET		 << 8 | DBG_VIP 
#define NET_FLOW				 DBG_BUSINESS_NET		 << 8 | DBG_FLOW 
#define NET_IMPORTANT			 DBG_BUSINESS_NET		 << 8 | DBG_IMPORTANT 
#define NET_DETAIL				 DBG_BUSINESS_NET		 << 8 | DBG_DETAIL 

#define PA_PANIC				 DBG_BUSINESS_PA		 << 8 | DBG_DBG_PANIC 
#define PA_FATAL				 DBG_BUSINESS_PA		 << 8 | DBG_FATAL 
#define PA_ERROR				 DBG_BUSINESS_PA		 << 8 | DBG_ERROR 
#define PA_WARNNING			 	 DBG_BUSINESS_PA		 << 8 | DBG_WARNNING 
#define PA_SUC					 DBG_BUSINESS_PA		 << 8 | DBG_SUC 
#define PA_CORRECT				 DBG_BUSINESS_PA		 << 8 | DBG_CORRECT 
#define PA_VIP 				 	 DBG_BUSINESS_PA		 << 8 | DBG_VIP 
#define PA_FLOW				 	 DBG_BUSINESS_PA		 << 8 | DBG_FLOW 
#define PA_IMPORTANT			 DBG_BUSINESS_PA		 << 8 | DBG_IMPORTANT 
#define PA_DETAIL				 DBG_BUSINESS_PA		 << 8 | DBG_DETAIL 

#define ARGS_ARGSNIC			 DBG_BUSINESS_ARGS		 << 8 | DBG_DBG_ARGSNIC 
#define ARGS_FATAL				 DBG_BUSINESS_ARGS		 << 8 | DBG_FATAL 
#define ARGS_ERROR				 DBG_BUSINESS_ARGS		 << 8 | DBG_ERROR 
#define ARGS_WARNNING			 DBG_BUSINESS_ARGS		 << 8 | DBG_WARNNING 
#define ARGS_SUC				 DBG_BUSINESS_ARGS		 << 8 | DBG_SUC 
#define ARGS_CORRECT			 DBG_BUSINESS_ARGS		 << 8 | DBG_CORRECT 
#define ARGS_VIP 				 DBG_BUSINESS_ARGS		 << 8 | DBG_VIP 
#define ARGS_FLOW				 DBG_BUSINESS_ARGS		 << 8 | DBG_FLOW 
#define ARGS_IMPORTANT			 DBG_BUSINESS_ARGS		 << 8 | DBG_IMPORTANT 
#define ARGS_DETAIL				 DBG_BUSINESS_ARGS		 << 8 | DBG_DETAIL 
#endif
