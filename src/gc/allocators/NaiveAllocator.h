#pragma once

#include "gc/allocators/Allocator.h"

#include <mutex>
#include <unordered_set>

namespace gccpp {
    class NaiveAllocator: public Allocator {
    public:
        NaiveAllocator() = default;
        ~NaiveAllocator() override;

    public:
        void* alloc(std::size_t size) override;
        void free(void* addr) override;

        std::unordered_set<void*>& alloc_stat() { //todo replace to erase????
            return allocated;
        }
    private:
        std::unordered_set<void*> allocated;
        std::mutex lock;
    };
}
