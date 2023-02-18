#include "LinearAllocator.h"

#include <cstdlib>
#include <cstdio>
#include <exception>
#include <cstring>
#include <cassert>

namespace gccpp {
    LinearAllocator::~LinearAllocator() {
       std::free(start_ptr);
    }

    void *LinearAllocator::alloc(std::size_t size) {
        const std::lock_guard<std::mutex> _l(lock);

        const std::size_t current_address = reinterpret_cast<std::size_t>(start_ptr) + offset;
        const std::size_t aligned_size = align(size + sizeof(Header));
        if (aligned_size + offset > max_size) {
            return nullptr;
        }
        auto* header = reinterpret_cast<Header*>(current_address);
        header->magic = MAGIC;
        assert(std::in_range<int>(aligned_size));
        header->chunk_size = static_cast<int>(aligned_size);

        //Zeroed object header.
        std::memset(reinterpret_cast<void*>(current_address + sizeof(Header)), 0, 8);

        offset += aligned_size;
        allocation_count += 1;
        return reinterpret_cast<void*>(current_address + sizeof(Header));
    }

    void LinearAllocator::free(void *addr) {
        printf("LinearAllocator::free is unreachable.\n");
        std::terminate();
    }

    LinearAllocator::LinearAllocator(std::size_t _max_size):
        max_size(align(_max_size)) {
        start_ptr = std::aligned_alloc(sizeof(std::size_t), max_size);
    }

    void LinearAllocator::release() noexcept {
#ifndef NDEBUG
        std::memset(start_ptr, 0, offset); // for debugging
#endif
        offset = 0;
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

