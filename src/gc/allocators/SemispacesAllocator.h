#pragma once

#include "gc/allocators/Allocator.h"
#include "gc/allocators/LinearAllocator.h"

#include <utility>

namespace gccpp {
    class SemispacesAllocator final: public Allocator {
    public:
        explicit SemispacesAllocator(std::size_t max_size) :
                active_space(new LinearAllocator(max_size)),
                free_space(new LinearAllocator(max_size)) {}

        ~SemispacesAllocator() override;
    public:
        void* alloc(std::size_t size) noexcept override;
        void free(void* addr) noexcept override;
        void print(std::ostringstream &out) override;

    public:
        void change_space() noexcept {
            std::swap(active_space, free_space);
        }

    public: //todo public temporally
        LinearAllocator *active_space;
        LinearAllocator *free_space;
    };
}