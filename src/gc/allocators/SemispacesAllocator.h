#pragma once

#include "gc/allocators/Allocator.h"
#include "gc/allocators/details/LinearAllocator.h"
#include "gc/allocators/details/FixedSizeAllocator.h"

#include <utility>

namespace gccpp {
    class SemispacesAllocator final: public Allocator {
    public:
        explicit SemispacesAllocator(std::size_t max_size) :
                active_space(new LinearAllocator(max_size)),
                free_space(new LinearAllocator(max_size)),
                small_allocator32(4096) {}

        ~SemispacesAllocator() override;
    public:
        void* alloc(std::size_t size) noexcept override;
        void print(std::ostringstream &out) override;

    public:
        void change_space() noexcept {
            std::swap(active_space, free_space);
        }

    public: //todo public temporally
        LinearAllocator *active_space;
        LinearAllocator *free_space;
        details::FixedSizeAllocator<32> small_allocator32;
        //details::FixedSizeAllocator<64> small_allocator64;
    };
}