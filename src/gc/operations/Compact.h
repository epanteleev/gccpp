#pragma once

#include <stack>
#include "gc/operations/GCOperation.h"
#include "gc/allocators/NaiveAllocator.h"

namespace gccpp::details {
    class Compact : public GCOperation {
    public:
        explicit Compact(NaiveAllocator& _allocator): allocator(_allocator) {}

        void trace(details::ObjectPointer ptr) override {
            void* new_addr = reinterpret_cast<void*>(*reinterpret_cast<std::size_t*>(ptr.mw()));
            ptr.update(new_addr);

            worklist.push(ptr);
        };
        void do_it(GC* gc) override;

    private:
        std::stack<details::ObjectPointer> worklist;
        NaiveAllocator& allocator;
    };
}
