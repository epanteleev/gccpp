#include "LinearAllocator.h"

#include <cstdlib>
#include <cstdio>
#include <exception>

namespace gccpp {
    LinearAllocator::~LinearAllocator() {
       _aligned_free(start_ptr);
    }

    void *LinearAllocator::alloc(std::size_t size) {
        const std::size_t current_address = reinterpret_cast<std::size_t>(start_ptr) + offset;
        const std::size_t aligned_size = align(size);
        if (aligned_size + offset > max_size) {
            return nullptr;
        }

        offset += aligned_size;
        return reinterpret_cast<void*>(current_address);
    }

    void LinearAllocator::free(void *addr) {
        printf("LinearAllocator::free is unreachable.\n");
        std::terminate();
    }

    LinearAllocator::LinearAllocator(std::size_t _max_size):
        max_size(align(_max_size)) {
        start_ptr = _aligned_malloc(max_size, sizeof(std::size_t));
    }

    static_assert(sizeof(std::size_t) == 8);
}

