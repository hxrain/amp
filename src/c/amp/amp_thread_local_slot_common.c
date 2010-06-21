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
 * Cross platform functionality shared by all platform backends.
 * Look into the different amp_raw_thread_local_slot_  backend source files for
 * the platform / backend specific implementations.
 */


#include "amp_thread_local_slot.h"

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include "amp_stddef.h"
#include "amp_raw_thread_local_slot.h"



int amp_thread_local_slot_create(amp_thread_local_slot_key_t* key,
                                 void* allocator_context,
                                 amp_alloc_func_t alloc_func,
                                 amp_dealloc_func_t dealloc_func)
{
    assert(NULL != key);
    assert(NULL != alloc_func);
    assert(NULL != dealloc_func);
    
    *key = AMP_THREAD_LOCAL_SLOT_UNINITIALIZED;
    
    if (NULL == key
        || NULL == alloc_func
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    amp_thread_local_slot_key_t tmp_key = (amp_thread_local_slot_key_t)alloc_func(allocator_context,
                                                                                  sizeof(struct amp_raw_thread_local_slot_key_s));
    if (NULL == tmp_key) {
        return ENOMEM;
    }
    
    int const retval = amp_raw_thread_local_slot_init(tmp_key);
    if (AMP_SUCCESS == retval) {
        *key = tmp_key;
    } else {
        int const rc = dealloc_func(allocator_context,
                                    tmp_key);
        assert(AMP_SUCCESS == rc);
    }
    
    return retval;
}



int amp_thread_local_slot_destroy(amp_thread_local_slot_key_t* key,
                                  void* allocator_context,
                                  amp_dealloc_func_t dealloc_func)
{
    assert(NULL != key);
    assert(NULL != *key);
    assert(NULL != dealloc_func);
    
    if (NULL == key
        || NULL == *key
        || NULL == dealloc_func) {
        
        return EINVAL;
    }
    
    int retval = amp_raw_thread_local_slot_finalize(*key);
    if (AMP_SUCCESS == retval) {
        retval = dealloc_func(allocator_context,
                              *key);
        assert(AMP_SUCCESS == retval);
        if (AMP_SUCCESS == retval) {
            *key = AMP_THREAD_LOCAL_SLOT_UNINITIALIZED;
        }
    }
    
    return retval;
}


