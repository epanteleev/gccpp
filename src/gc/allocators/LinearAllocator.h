#pragma once
#include "gc/allocators/Allocator.h"

namespace gccpp {

    class LinearAllocator: public Allocator {
    public:
        explicit LinearAllocator(std::size_t _max_size_in_byte);
        ~LinearAllocator() override;
    public:
        void* alloc(std::size_t size) override;
        void free(void* addr) override;

        void release() noexcept {
            offset = 0;
        }
    private:
        static std::size_t align(std::size_t value) noexcept {
            return ((value + 7) / 8) * 8;
        }

    private:
        void* start_ptr{};
        std::size_t offset{};
        std::size_t max_size{};
    };
}


