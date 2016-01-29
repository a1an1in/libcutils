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
	MAX_DEBUG_BUSINESS_NUM
};
/*
 *enum debug_level_enum{
 *    DBG_PANIC=0, 		
 *    DBG_FATAL, 		
 *    DBG_ERROR ,		
 *    DBG_WARNNING,
 *    DBG_SUC ,		
 *    DBG_CORRECT,
 *    DBG_VIP, 	
 *    DBG_FLOW,
 *    DBG_IMPORTANT, 		
 *    DBG_DETAIL,
 *    DBG_MAX_LEVEL
 *};
 */
#define DBG_ALLOC_DBG_PANIC		 DBG_BUSINESS_ALLOC << 8 | DBG_DBG_PANIC 
#define DBG_ALLOC_FATAL			 DBG_BUSINESS_ALLOC << 8 | DBG_FATAL 
#define DBG_ALLOC_ERROR			 DBG_BUSINESS_ALLOC << 8 | DBG_ERROR 
#define DBG_ALLOC_WARNNING		 DBG_BUSINESS_ALLOC << 8 | DBG_WARNNING 
#define DBG_ALLOC_SUC			 DBG_BUSINESS_ALLOC << 8 | DBG_SUC 
#define DBG_ALLOC_CORRECT		 DBG_BUSINESS_ALLOC << 8 | DBG_CORRECT 
#define DBG_ALLOC_VIP 		  	 DBG_BUSINESS_ALLOC << 8 | DBG_VIP 
#define DBG_ALLOC_FLOW			 DBG_BUSINESS_ALLOC << 8 | DBG_FLOW 
#define DBG_ALLOC_IMPORTANT		 DBG_BUSINESS_ALLOC << 8 | DBG_IMPORTANT 
#define DBG_ALLOC_DETAIL		 DBG_BUSINESS_ALLOC << 8 | DBG_DETAIL 


#endif
