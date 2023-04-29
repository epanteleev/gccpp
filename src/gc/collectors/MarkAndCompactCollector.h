#pragma once
#include "gc/allocators/Allocator.h"

#include "gc/collectors/BasicCollector.h"
#include "gc/allocators/SemispacesAllocator.h"
#include "gc/containers/Buffer.h"

#include <cstring>
#include <cstdio>

namespace gccpp {
    namespace details {
        class Mark;
        class Reallocate;
    }

    class MarkAndCompactCollector: public BasicCollector {
    public:
        explicit MarkAndCompactCollector(std::size_t heap_size):
            BasicCollector(new SemispacesAllocator(heap_size)) {}

        ~MarkAndCompactCollector() override {
            delete allocator;
        }

    public:
        void collect() override;
    private:
        details::Buffer<details::ObjectPointer*> worklist{};
    };
}
