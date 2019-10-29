// Copyright (c) 2019 Doug Binks
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "TaskScheduler_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

enkiTaskScheduler*    pETS;
enkiTaskSet*          pParallelTask;

size_t totalAllocations = 0;

void ParallelFunc( uint32_t start_, uint32_t end, uint32_t threadnum_, void* pArgs_ )
{
    // do something
    printf("ParallelFunc running on thread %d (could be any thread)\n", threadnum_ );
}

void* CustomAllocFunc( size_t align_, size_t size_, void* userData_, const char* file_, int line_ )
{
    (void)align_; // for example ignoring alignment
    totalAllocations += size_;
    printf("Allocating %g bytes in domain %s, total %g. File %s, line %d.\n",
        (double)size_, (const char*)userData_, (double)totalAllocations, file_, line_ );
    return enkiDefaultAllocFunc( align_, size_, userData_, file_, line_ );
};

void  CustomFreeFunc(  void* ptr_,   void* userData_, const char* file_, int line_ )
{
    printf("Freeing %p in domain %s. File %s, line %d.\n",
        ptr_, (const char*)userData_, file_, line_ );
    enkiDefaultFreeFunc( ptr_, userData_, file_, line_ );
};


int main(int argc, const char * argv[])
{
    struct enkiCustomAllocator customAllocator;

    customAllocator.alloc = CustomAllocFunc;
    customAllocator.free  = CustomFreeFunc;
    customAllocator.userData = (void*)"enkiTS";

    pETS = enkiNewTaskSchedulerWithCustomAllocator( customAllocator );
    enkiInitTaskScheduler( pETS );

    pParallelTask = enkiCreateTaskSet( pETS, ParallelFunc );

    enkiAddTaskSetToPipe( pETS, pParallelTask, NULL, 1);
    enkiWaitForTaskSet( pETS, pParallelTask );

    enkiDeleteTaskSet( pParallelTask );

    enkiDeleteTaskScheduler( pETS );
}