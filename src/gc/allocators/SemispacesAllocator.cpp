#include "SemispacesAllocator.h"

#include <cstdio>
#include <exception>

namespace gccpp {
    SemispacesAllocator::~SemispacesAllocator() {
        delete active_space;
        delete free_space;
    }

    void *SemispacesAllocator::alloc(std::size_t size) {
        return active_space->alloc(size);
    }

    void SemispacesAllocator::free(void *addr) {
        printf("SemispacesAllocator::free is unreachable.\n");
        std::terminate();
    }
}