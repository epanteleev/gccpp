#include <cassert>
#include <cstdio>
#include <malloc.h>
#include <sstream>
#include <functional>

#include "gc/allocators/MallocBasedAllocator.h"

namespace gccpp {
    void *MallocBasedAllocator::alloc(std::size_t size) noexcept {
        const std::lock_guard _l(lock);
        void* ptr = std::malloc(size + sizeof(malloc_based::Chunk));
        assert(ptr != nullptr);

        std::size_t actual_size = malloc_usable_size(ptr); //Todo linux specific

        if (max_size < stat.allocated_size + actual_size) {
            std::free(ptr);
            return nullptr;
        }

        auto ch = malloc_based::Chunk::make(ptr);
        allocInfo.append(ch);
        stat.allocation_count += 1;
        stat.allocated_size += actual_size;

        return ch->object();
    }

    void MallocBasedAllocator::free(void* addr) noexcept {
        assert(addr!= nullptr);
        //Todo assert(lock.is_unlocked());

        stat.allocation_count -= 1;
        stat.allocated_size -= malloc_usable_size(addr);
        std::free(addr);
    }

    MallocBasedAllocator::~MallocBasedAllocator() {
        auto i = allocInfo.first;
        while (i != nullptr) {
            auto curr = i;
            i = i->next;
            std::free(curr);
        }
    }

    void MallocBasedAllocator::print(std::ostringstream &out) {
        out << "Allocation stat:\n"
            << "Max size: " << max_size << " byte\n"
            << "Allocation count: " << stat.allocation_count << " byte\n"
            << "Total size: " << stat.allocated_size << " byte\n";
    }

    void MallocBasedAllocator::free_if(const std::function<bool(void *)> &function) {
        auto i = allocInfo.first;
        while (i != nullptr) {
            if (function(i->object())) {
                auto next = allocInfo.remove(i);
                free(i);
                i = next;
            } else {
                i = i->next;
            }
        }
    }
}