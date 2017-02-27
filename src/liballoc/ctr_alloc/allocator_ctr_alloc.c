/*
 * =====================================================================================
 *
 *       Filename:  allocator_ctr_alloc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/15/2015 09:51:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alan lin (), a1an1in@sina.com
 *   Organization:  
 *
 * =====================================================================================
 */
/*  
 * Copyright (c) 2015-2020 alan lin <a1an1in@sina.com>
 *  
 *  
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "libdbg/debug.h"
#include "liballoc/inc_files.h"

static int __init(allocator_t *allocator)
{
    ctr_alloc_t *ctr_alloc = &allocator->priv.ctr_alloc;
    ctr_mempool_t *mempool;
    struct list_head **free_slabs,**used_slabs;
    int slab_array_max_num;
    int i;

    if (!ctr_alloc->slab_array_max_num) {
        ctr_alloc->slab_array_max_num = SLAB_ARRAY_MAX_NUM;
    }
    if (!ctr_alloc->data_min_size) {
        ctr_alloc->data_min_size = 8;
    }
    if (!ctr_alloc->mempool_capacity) {
        ctr_alloc->mempool_capacity = MEM_POOL_MAX_SIZE;
    }
    allocator->alloc_count = 0;

    dbg_str(ALLOC_DETAIL,"slab_num=%d,data_size=%d,cap=%d",
            ctr_alloc->slab_array_max_num,
            ctr_alloc->data_min_size,
            ctr_alloc->mempool_capacity);

    slab_array_max_num = ctr_alloc->slab_array_max_num;

    free_slabs = (struct list_head **)malloc(slab_array_max_num * sizeof(int *));
    if (free_slabs == NULL) {
        dbg_str(DBG_ERROR,"__init");
        return -1;
    } else {
        ctr_alloc->used_slabs = free_slabs;
    }

    used_slabs = (struct list_head **)malloc(slab_array_max_num * sizeof(int *));
    if (used_slabs == NULL) {
        dbg_str(DBG_ERROR,"__init");
        return -1;
    } else {
        ctr_alloc->free_slabs = used_slabs;
    }

    dbg_str(DBG_DETAIL,"lock_type=%d",allocator->lock_type);
    for (i = 0; i < slab_array_max_num; i++) {
        slab_init_head_list(&used_slabs[i],allocator->lock_type);
        slab_init_head_list(&free_slabs[i],allocator->lock_type);
    }

    mempool_init_head_list(&ctr_alloc->pool,allocator->lock_type);
    mempool = mempool_create_list(allocator);
    if (mempool == NULL) {
        ctr_alloc->pool = NULL;
    } else {
        mempool_attach_list(&mempool->list_head,ctr_alloc->pool);
    }

    mempool_init_head_list(&ctr_alloc->empty_pool,allocator->lock_type);

    return 0;
}

static void *alloc_huge_slab(allocator_t *allocator,uint32_t size)
{
    dbg_str(DBG_DETAIL,"alloc_huge_slab");
    allocator->alloc_count++;

    return malloc(size);
}

static void *alloc_normal_slab(allocator_t *allocator,uint32_t size)
{
    ctr_slab_t *slab_list;
    uint32_t index;

    if (!(slab_list = slab_detach_front_list_from_free_slabs(allocator,size))) {
        if (!(slab_list = mempool_alloc_slab_list(allocator,size))) {  
            dbg_str(DBG_ERROR,"allocator slab list err");
            return NULL;
        }
    }

    slab_attach_list_to_used_slabs(allocator,&slab_list->list_head,size);

    allocator->alloc_count++;

    dbg_str(ALLOC_IMPORTANT,"ctr_alloc allocator mem,request size=%d,mem addr=%p,"
            "allocator using count=%d",size,slab_list->mem_addr,allocator->alloc_count);

    return slab_list->mem_addr;
}

static void *__alloc(allocator_t *allocator,uint32_t size)
{
    uint32_t index;
    ctr_alloc_t *ctr_alloc = &allocator->priv.ctr_alloc;
    void *mem;

    index = slab_get_slab_index(allocator,size);
    /*
     *dbg_str(ALLOC_DETAIL,"allocator mem,size=%d,index=%d,slab_array_max_num=%d",
     *        size,index,allocator->priv.ctr_alloc.slab_array_max_num);
     */
    if (size > ctr_alloc->mempool_capacity || index >= ctr_alloc->slab_array_max_num) {
        dbg_str(DBG_WARNNING,"ctr alloc size = %d, which is too huge, using sys malloc", size);
        mem = alloc_huge_slab(allocator,size);
        return mem;
    }

    mem = alloc_normal_slab(allocator,size);

    return mem;
}

static free_huge_slab(allocator_t *allocator,void *addr)
{
    free(addr);
    allocator->alloc_count--;
}

static free_normal_slab(allocator_t *allocator,ctr_slab_t *slab_list)
{
    ctr_alloc_t *ctr_alloc = &allocator->priv.ctr_alloc;
    struct list_head *new_head;
    struct list_head **free_slabs;
    uint32_t size;

    new_head = &slab_list->list_head;
    size = slab_list->size;
    free_slabs = ctr_alloc->free_slabs;

    slab_detach_list_from_used_slabs(allocator,
                                     new_head,//struct list_head *del_head,
                                     size);//uint32_t size);
    slab_list->stat_flag = 0;
    slab_attach_list_to_free_slabs(allocator,
                                   new_head,//struct list_head *new_head,
                                   size);//uint32_t size);

    allocator->alloc_count--;

}

static void __free(allocator_t *allocator,void *addr)
{
    ctr_alloc_t *ctr_alloc = &allocator->priv.ctr_alloc;
    ctr_slab_t *slab_list;
    struct list_head *new_head;
    struct list_head **free_slabs;
    uint32_t size;
    uint32_t index;

    if (addr == NULL) {
        dbg_str(ALLOC_DETAIL,"release addr is NULL");
        return;
    }
    slab_list = container_of(addr,ctr_slab_t,data);
    if (slab_list->stat_flag == 0) {
        dbg_str(DBG_WARNNING,"this addr has been released");
        return;
    }

    size = slab_list->size;
    index = slab_get_slab_index(allocator,size);
    if (size >= ctr_alloc->mempool_capacity || index >= ctr_alloc->slab_array_max_num) {
        free_huge_slab(allocator,addr);
        return;
    }

    /*
     *dbg_str(ALLOC_DETAIL,"release slab_list:%p",slab_list);
     *dbg_str(ALLOC_DETAIL,"release mem_addr:%p,addr:%p",slab_list->mem_addr,addr);
     */

    free_normal_slab(allocator,slab_list);

    dbg_str(ALLOC_IMPORTANT,"free ctr mem,free add=%p,size =%d, allocator using count=%d",
            addr,slab_list->data_size, allocator->alloc_count);

}

static void __info(allocator_t *allocator)
{
    int i;
    int slab_array_max_num = allocator->priv.ctr_alloc.slab_array_max_num;

    printf("##########################-printf allocator mem info##########################");
    dbg_str(ALLOC_DETAIL,"the mem using, count=%d",allocator->alloc_count);
    dbg_str(ALLOC_DETAIL,"query pool:");
    mempool_print_list_for_each(allocator->priv.ctr_alloc.pool);

    dbg_str(ALLOC_DETAIL,"query empty_pool:");
    mempool_print_list_for_each(allocator->priv.ctr_alloc.empty_pool);

    dbg_str(ALLOC_DETAIL,"query free_slabs:");
    for(i = 0; i < slab_array_max_num; i++) {
        slab_print_list_for_each(allocator->priv.ctr_alloc.free_slabs[i],i);
    }

    dbg_str(ALLOC_DETAIL,"query used_slabs:");
    for(i = 0; i < slab_array_max_num; i++) {
        slab_print_list_for_each(allocator->priv.ctr_alloc.used_slabs[i],i);
    }
    printf("##############################################################################\n");
}

static void __destroy(allocator_t *allocator)
{
    int i;
    int slab_array_max_num        = allocator->priv.ctr_alloc.slab_array_max_num;
    struct list_head **free_slabs = allocator->priv.ctr_alloc.free_slabs;
    struct list_head **used_slabs = allocator->priv.ctr_alloc.used_slabs;

    dbg_str(ALLOC_DETAIL,"destroy pool");
    mempool_destroy_lists(allocator->priv.ctr_alloc.pool);
    dbg_str(ALLOC_DETAIL,"destroy empty_pool");
    mempool_destroy_lists(allocator->priv.ctr_alloc.empty_pool);

    dbg_str(ALLOC_DETAIL,"destroy free_slabs");
    for(i = 0; i < slab_array_max_num; i++) {
        slab_release_head_list(free_slabs[i]);
        slab_release_head_list(used_slabs[i]);
    }
    free(used_slabs);
    free(free_slabs);
}

int allocator_ctr_alloc_register() {
    allocator_module_t salloc = {
        .allocator_type = ALLOCATOR_TYPE_CTR_MALLOC,
        .alloc_ops = {
            .init    = __init,
            .alloc   = __alloc,
            .free    = __free,
            .destroy = __destroy,
            .info    = __info
        }
    };
    memcpy(&allocator_modules[ALLOCATOR_TYPE_CTR_MALLOC],
           &salloc,sizeof(allocator_module_t));

    return 0;
}