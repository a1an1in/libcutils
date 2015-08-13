/*
 * =====================================================================================
 *
 *       Filename:  container_vector.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/12/2015 09:32:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "basic_types.h"
#ifndef __CONTAINER_VECTOR_H__
#define __CONTAINER_VECTOR_H__
typedef struct vector_priv_info{
	uint32_t data_size;
	uint32_t capacity;
	uint32_t step;
	void *vector_head;
}vector_priv_t;

int container_vector_register();
#endif
