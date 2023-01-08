#include <cstdio>
#include "Allocator.h"

namespace tgc {
    void *NaiveAllocator::alloc(std::size_t size) {
        void* ptr = std::malloc(size);
        allocated.push_back(ptr);
        return ptr;
    }

    void NaiveAllocator::free_dead(std::vector<void*>&& live_set) {
        std::vector<void*> dead;
        std::sort(allocated.begin(), allocated.end());
        std::sort(live_set.begin(), live_set.end());

        std::set_difference(allocated.begin(), allocated.end(), live_set.begin(), live_set.end(),
                            std::back_inserter(dead));

        for(auto* d : dead) {
            std::free(d);
        }
        allocated = std::move(live_set);
    }

    NaiveAllocator::~NaiveAllocator() {
        for(auto& i: allocated) {
            std::free(i);
        }
    }
}