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

int ctr_alloc_init(allocator_t *alloc)
{
    ctr_alloc_t *alloc_p = &alloc->priv.ctr_alloc;
    ctr_mempool_t *mempool;
    struct list_head **free_slabs,**used_slabs;
    int slab_array_max_num;
    int i;

    dbg_str(ALLOC_DETAIL,"in ctr_alloc_init");

    if(!alloc_p->slab_array_max_num) {
        alloc_p->slab_array_max_num = SLAB_ARRAY_MAX_NUM;
    }
    if(!alloc_p->data_min_size){
        alloc_p->data_min_size = 8;
    }
    if(!alloc_p->mempool_capacity){
        alloc_p->mempool_capacity = MEM_POOL_MAX_SIZE;
    }
    alloc->alloc_count = 0;

    dbg_str(ALLOC_DETAIL,"slab_num=%d,data_size=%d,cap=%d",alloc_p->slab_array_max_num,alloc_p->data_min_size,alloc_p->mempool_capacity);

    slab_array_max_num = alloc_p->slab_array_max_num;

    free_slabs = (struct list_head **)malloc(slab_array_max_num * sizeof(int *));
    if(free_slabs == NULL){
        dbg_str(DBG_ERROR,"ctr_alloc_init");
        return -1;
    }else{
        alloc_p->used_slabs = free_slabs;
    }
    used_slabs = (struct list_head **)malloc(slab_array_max_num * sizeof(int *));
    if(used_slabs == NULL){
        dbg_str(DBG_ERROR,"ctr_alloc_init");
        return -1;
    }else{
        alloc_p->free_slabs = used_slabs;
    }

    dbg_str(DBG_DETAIL,"lock_type=%d",alloc->lock_type);
    for(i = 0; i < slab_array_max_num; i++){
        slab_init_head_list(&used_slabs[i],alloc->lock_type);
        slab_init_head_list(&free_slabs[i],alloc->lock_type);
    }

    mempool_init_head_list(&alloc_p->pool,alloc->lock_type);
    mempool = mempool_create_list(alloc);
    if(mempool == NULL){
        alloc_p->pool = NULL;
    }else{
        mempool_attach_list(&mempool->list_head,alloc_p->pool);
    }

    mempool_init_head_list(&alloc_p->empty_pool,alloc->lock_type);

    dbg_str(ALLOC_DETAIL,"out ctr_alloc_init");

    return 0;
}
void *ctr_alloc_alloc(allocator_t *alloc,uint32_t size)
{
    ctr_slab_t *slab_list;
    uint32_t index;

    index = slab_get_slab_index(alloc,size);
    /*
     *dbg_str(ALLOC_DETAIL,"alloc mem,size=%d,index=%d,slab_array_max_num=%d",
     *        size,index,alloc->priv.ctr_alloc.slab_array_max_num);
     */
    if(size > alloc->priv.ctr_alloc.mempool_capacity){
        dbg_str(DBG_ERROR,"apply szie excess mempool_capacity");
        return NULL;
    }
    if(index >= alloc->priv.ctr_alloc.slab_array_max_num){
        dbg_str(DBG_ERROR,"apply size:%d too large,max size:%d,excess slab num,"
                "please assignd by sys malloc,or reconfig ctr_alloc",size,20*8);
        return NULL;
    }

    if(!(slab_list = slab_detach_front_list_from_free_slabs(alloc,size))){
        if(!(slab_list = mempool_alloc_slab_list(alloc,size))){  
            dbg_str(DBG_ERROR,"alloc slab list err");
            return NULL;
        }
    }
    slab_attach_list_to_used_slabs(alloc,&slab_list->list_head,size);
    alloc->alloc_count++;
    dbg_str(ALLOC_IMPORTANT,"ctr_alloc alloc mem,request size=%d,mem addr=%p,"
            "alloc using count=%d",size,slab_list->mem_addr,alloc->alloc_count);

    return slab_list->mem_addr;
}
void ctr_alloc_free(allocator_t *alloc,void *addr)
{
    ctr_slab_t *slab_list;
    struct list_head *new_head;
    struct list_head **free_slabs;
    uint32_t size;
    ctr_alloc_t *alloc_p = &alloc->priv.ctr_alloc;

    if(addr == NULL){
        dbg_str(ALLOC_DETAIL,"release addr is NULL");
        return;
    }
    slab_list = container_of(addr,ctr_slab_t,data);
    if(slab_list->stat_flag == 0){
        dbg_str(DBG_WARNNING,"this addr has been released");
        return;
    }

    /*
     *dbg_str(ALLOC_DETAIL,"release slab_list:%p",slab_list);
     *dbg_str(ALLOC_DETAIL,"release mem_addr:%p,addr:%p",slab_list->mem_addr,addr);
     */
    alloc->alloc_count--;

    new_head = &slab_list->list_head;
    size = slab_list->size;
    free_slabs = alloc_p->free_slabs;

    slab_detach_list_from_used_slabs(alloc,
            new_head,//struct list_head *del_head,
            size);//uint32_t size);
    slab_list->stat_flag = 0;
    slab_attach_list_to_free_slabs(alloc,
            new_head,//struct list_head *new_head,
            size);//uint32_t size);
    dbg_str(ALLOC_IMPORTANT,"free ctr mem,free add=%p,alloc using count=%d",addr,alloc->alloc_count);
}
void ctr_alloc_info(allocator_t *alloc)
{
    int i;
    int slab_array_max_num = alloc->priv.ctr_alloc.slab_array_max_num;

    printf("##########################-printf allocator mem info##########################");
    dbg_str(ALLOC_DETAIL,"the mem using, count=%d",alloc->alloc_count);
    dbg_str(ALLOC_DETAIL,"query pool:");
    mempool_print_list_for_each(alloc->priv.ctr_alloc.pool);

    dbg_str(ALLOC_DETAIL,"query empty_pool:");
    mempool_print_list_for_each(alloc->priv.ctr_alloc.empty_pool);

    dbg_str(ALLOC_DETAIL,"query free_slabs:");
    for(i = 0; i < slab_array_max_num; i++){
        slab_print_list_for_each(alloc->priv.ctr_alloc.free_slabs[i],i);
    }

    dbg_str(ALLOC_DETAIL,"query used_slabs:");
    for(i = 0; i < slab_array_max_num; i++){
        slab_print_list_for_each(alloc->priv.ctr_alloc.used_slabs[i],i);
    }
    printf("##############################################################################\n");
}
void ctr_alloc_destroy(allocator_t *alloc)
{
    int i;
    int slab_array_max_num = alloc->priv.ctr_alloc.slab_array_max_num;
    struct list_head **free_slabs = alloc->priv.ctr_alloc.free_slabs;
    struct list_head **used_slabs = alloc->priv.ctr_alloc.used_slabs;

    dbg_str(ALLOC_DETAIL,"destroy pool");
    mempool_destroy_lists(alloc->priv.ctr_alloc.pool);
    dbg_str(ALLOC_DETAIL,"destroy empty_pool");
    mempool_destroy_lists(alloc->priv.ctr_alloc.empty_pool);

    dbg_str(ALLOC_DETAIL,"destroy free_slabs");
    for(i = 0; i < slab_array_max_num; i++){
        slab_release_head_list(free_slabs[i]);
        slab_release_head_list(used_slabs[i]);
    }
    free(used_slabs);
    free(free_slabs);
}

int allocator_ctr_alloc_register(){
    allocator_module_t salloc = {
        .allocator_type = ALLOCATOR_TYPE_CTR_MALLOC,
        .alloc_ops = {
            .init    = ctr_alloc_init,
            .alloc   = ctr_alloc_alloc,
            .free    = ctr_alloc_free,
            .destroy = ctr_alloc_destroy,
            .info    = ctr_alloc_info
        }
    };
    memcpy(&allocator_modules[ALLOCATOR_TYPE_CTR_MALLOC],&salloc,sizeof(allocator_module_t));
    return 0;
}
