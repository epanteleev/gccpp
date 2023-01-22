#include "LinearAllocator.h"

#include <cstdlib>
#include <cstdio>
#include <exception>
#include <cstring>
#include <utility>
#include <cassert>

namespace gccpp {
    LinearAllocator::~LinearAllocator() {
       _aligned_free(start_ptr);
    }

    void *LinearAllocator::alloc(std::size_t size) {
        const std::size_t current_address = reinterpret_cast<std::size_t>(start_ptr) + offset;
        const std::size_t aligned_size = align(size + sizeof(Header));
        if (aligned_size + offset > max_size) {
            return nullptr;
        }
        auto* header = reinterpret_cast<Header*>(current_address);
        header->magic = MAGIC;
        assert(std::in_range<int>(aligned_size));
        header->chunk_size = static_cast<int>(aligned_size);

        offset += aligned_size;
        allocation_count += 1;
        last_allocated_object = reinterpret_cast<void*>(current_address + sizeof(Header));
        return last_allocated_object;
    }

    void LinearAllocator::free(void *addr) {
        printf("LinearAllocator::free is unreachable.\n");
        std::terminate();
    }

    LinearAllocator::LinearAllocator(std::size_t _max_size):
        max_size(align(_max_size)) {
        start_ptr = _aligned_malloc(max_size, sizeof(std::size_t));
        last_allocated_object = static_cast<std::byte*>(start_ptr) + max_size;
    }

    void LinearAllocator::release() noexcept {
#ifndef NDEBUG
        std::memset(start_ptr, 0, offset); // for debugging
#endif
        offset = 0;
        last_allocated_object = nullptr;
        allocation_count = 0;
    }

    bool LinearAllocator::contains(void *object_address) const noexcept {
        const auto end = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(start_ptr) + offset);
        auto* header = reinterpret_cast<Header*>(reinterpret_cast<size_t>(object_address) - sizeof(Header));
        assert(header->magic == MAGIC);

        return start_ptr <= object_address && object_address < end;
    }

    Header *LinearAllocator::header(const void *object_address) noexcept {
        auto* header = reinterpret_cast<Header*>(reinterpret_cast<std::size_t>(object_address) - sizeof(Header));
        assert(header->magic == MAGIC);
        return header;
    }

    void LinearAllocator::visit(const std::function<void(Header *)>& fn) noexcept {
        if (allocation_count == 0) {
            return;
        }
        auto current = static_cast<std::byte*>(start_ptr);

        do {
            auto h = reinterpret_cast<Header*>(current);
            assert(h->magic == MAGIC);
            fn(h);

            current = reinterpret_cast<std::byte*>(current) + h->chunk_size;

        } while(current < static_cast<std::byte*>(start_ptr) + offset);
    }

    static_assert(sizeof(std::size_t) == 8);
}

