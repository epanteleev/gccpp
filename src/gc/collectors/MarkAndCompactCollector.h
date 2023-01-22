#pragma once
#include "gc/allocators/Allocator.h"

#include "gc/collectors/GC.h"
#include "gc/allocators/SemispacesAllocator.h"

#include <cstring>
#include <cstdio>

namespace gccpp {
    namespace details {
        class Mark;
        class Compact;
    }

    class MarkAndCompactCollector: public GC {
    public:
        explicit MarkAndCompactCollector(std::size_t heap_size): GC(new SemispacesAllocator(heap_size)) {}
        ~MarkAndCompactCollector() override {
            delete allocator;
        }

    public:
        void safepoint_at_poll() override;
    };
}
