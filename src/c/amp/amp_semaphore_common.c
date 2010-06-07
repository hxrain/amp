/*
 * Copyright (c) 2009-2010, Bjoern Knafla
 * http://www.bjoernknafla.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 *
 *   * Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Bjoern Knafla 
 *     Parallelization + AI + Gamedev Consulting nor the names of its 
 *     contributors may be used to endorse or promote products derived from 
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *
 * Implementation shared by all amp semaphore backends.
 */

#include "amp_semaphore.h"
#include "amp_raw_semaphore.h"

#include <assert.h>
#include <errno.h>

#include "amp_stddef.h"



int amp_semaphore_create(amp_semaphore_t *semaphore,
                         amp_semaphore_counter_t init_count,
                         void *allocator_context,
                         amp_alloc_func_t alloc_func,
                         amp_dealloc_func_t dealloc_func)
{
    assert(NULL != semaphore);
    assert((amp_semaphore_counter_t)0 <= init_count);
    assert(AMP_RAW_SEMAPHORE_COUNT_MAX >= (amp_raw_semaphore_counter_t)init_count);
    assert(NULL != alloc_func);
    assert(NULL != dealloc_func);
    
    if (NULL == semaphore
        || (amp_semaphore_counter_t)0 > init_count
        || AMP_RAW_SEMAPHORE_COUNT_MAX < (amp_raw_semaphore_counter_t)init_count
        || NULL == alloc_func
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    amp_semaphore_t tmp_sema = (amp_semaphore_t)alloc_func(allocator_context,
                                                           sizeof(struct amp_raw_semaphore_s));
    if (NULL == tmp_sema) {
        return ENOMEM;
    }
    
    int const retval = amp_raw_semaphore_init(tmp_sema,
                                              init_count);
    if (AMP_SUCCESS == retval) {
        *semaphore = tmp_sema;
    } else {
        dealloc_func(allocator_context, tmp_sema);
    }
    
    return retval;
}



int amp_semaphore_destroy(amp_semaphore_t semaphore,
                          void *allocator_context,
                          amp_dealloc_func_t dealloc_func)
{
    assert(NULL != semaphore);
    assert(NULL != dealloc_func);
    
    if (NULL == semaphore
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    int const retval = amp_raw_semaphore_finalize(semaphore);
    if (AMP_SUCCESS == retval) {
        dealloc_func(allocator_context,
                     semaphore);
    }
    
    return retval;
}

