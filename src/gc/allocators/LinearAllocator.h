#pragma once
#include "gc/allocators/Allocator.h"
#include <functional>

namespace gccpp {

    struct Header final {
        unsigned char magic;
        int chunk_size;
    };
    static_assert(sizeof(Header) == 8, "Expect given size of the structure.");

    class LinearAllocator final : public Allocator {
    public:
        friend class LAIterator;
    public:
        static constexpr unsigned char MAGIC = 0xAA;
    public:
        explicit LinearAllocator(std::size_t _max_size_in_byte);
        ~LinearAllocator() override;

    public:
        void* alloc(std::size_t size) override;
        void free(void* addr) override;
        void release() noexcept;
        bool contains(void* object_address) const noexcept;

    public:
        void visit(const std::function<void(Header*)>& fn) noexcept;

    public:
        static std::size_t align(std::size_t value) noexcept {
            return ((value + 7) / 8) * 8;
        }

        static Header* header(const void* object_address) noexcept;
    private:
        void* start_ptr{};
        const std::size_t max_size{};
        std::size_t offset{};
        void* last_allocated_object;
        std::size_t allocation_count{};
    };
}


