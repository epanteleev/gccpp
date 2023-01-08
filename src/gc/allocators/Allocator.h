#pragma once
#include <cstddef>

namespace gccpp {
    class Allocator {
    public:
        virtual ~Allocator() = default;
        virtual void* alloc(std::size_t size) = 0;
        virtual void free(void* addr) = 0;
    };
}
