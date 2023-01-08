#pragma once

#include <unordered_set>
#include "gc/allocators/Allocator.h"

namespace gccpp {
    class NaiveAllocator: public Allocator {
    public:
        NaiveAllocator() = default;
        ~NaiveAllocator() override;

    public:
        void* alloc(std::size_t size) override;
        void free(void* addr) override;

        std::unordered_set<void*>& alloc_stat() {
            return allocated;
        }
    private:
        std::unordered_set<void*> allocated;
    };
}
