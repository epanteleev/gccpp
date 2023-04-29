#include "gc/allocators/LinearAllocator.h"
#include "gc/containers/SpinLock.h"
#include "gc/containers/Page.h"
#include "gc/containers/Memory.h"

#include <cstdlib>
#include <cstdio>
#include <exception>
#include <cstring>
#include <cassert>
#include <utility>
#include <sstream>

namespace gccpp {
    LinearAllocator::~LinearAllocator() {
       Page::free(start_ptr, max_size);
    }

    void *LinearAllocator::alloc(std::size_t size) noexcept {
        const std::lock_guard<details::SpinLock> _l(lock);

        const std::size_t current_address = reinterpret_cast<std::size_t>(start_ptr) + offset;
        const std::size_t aligned_size = details::mem::align(size + sizeof(Chunk));
        if (aligned_size + offset > max_size) {
            return nullptr;
        }
        auto* header = reinterpret_cast<Chunk*>(current_address);
        header->magic = MAGIC;

        assert(std::in_range<int>(aligned_size));
        header->chunk_size = static_cast<int>(aligned_size);

        offset += aligned_size;
        allocation_count += 1;
        return reinterpret_cast<void*>(current_address + sizeof(Chunk));
    }

    void LinearAllocator::free(void *addr) noexcept {
        (void)(addr); //todo make special macros
        fprintf(stderr,"LinearAllocator::free is unreachable.\n");
        std::terminate();
    }

    LinearAllocator::LinearAllocator(std::size_t _max_size):
        max_size(details::mem::align(_max_size)) {
        start_ptr = Page::alloc(max_size);
        assert(reinterpret_cast<std::size_t>(start_ptr) % 8 == 0);
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
#ifndef NDEBUG
        LinearAllocator::header(object_address);
#endif
        return start_ptr <= object_address && object_address < end;
    }

    Chunk *LinearAllocator::header(const void *object_address) noexcept {
        auto* header = reinterpret_cast<Chunk*>(reinterpret_cast<std::size_t>(object_address) - sizeof(Chunk));
        assert(header->magic == MAGIC);
        return header;
    }

    void LinearAllocator::visit(const std::function<void(Chunk *)>& fn) noexcept {
        if (allocation_count == 0) {
            return;
        }
        auto current = static_cast<std::byte*>(start_ptr);

        do {
            auto h = reinterpret_cast<Chunk*>(current);
            assert(h->magic == MAGIC);
            fn(h);

            current = reinterpret_cast<std::byte*>(current) + h->chunk_size;
        } while(current < static_cast<std::byte*>(start_ptr) + offset);
    }

    void LinearAllocator::print(std::ostringstream &out) {
        out << "Start: " << start_ptr << "\n"
            << "Max size: " << max_size << "\n"
            << "Offset: " << offset << "\n"
            << "Allocation count: " << allocation_count << "\n";
    }

    static_assert(sizeof(std::size_t) == 8);
}

