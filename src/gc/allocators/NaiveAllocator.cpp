#include "gc/allocators/NaiveAllocator.h"

#include <cassert>
#include <cstdio>

namespace gccpp {
    void *NaiveAllocator::alloc(std::size_t size) {
        void* ptr = std::malloc(size);
        {
            std::lock_guard _l(lock);
            allocated.insert(ptr);
        }
        return ptr;
    }

    void NaiveAllocator::free(void* addr) {
        assert(addr!= nullptr);
        std::free(addr);
    }

    NaiveAllocator::~NaiveAllocator() {
        for(auto& i: allocated) {
            std::free(i);
        }
    }
}