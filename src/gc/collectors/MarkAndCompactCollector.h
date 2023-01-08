#pragma once
#include "gc/allocators/Allocator.h"

#include "gc/collectors/GC.h"
#include "gc/allocators/NaiveAllocator.h"

#include <cstring>
#include <cstdio>

namespace gccpp {
    namespace details {
        class Mark;
        class Compact;
    }

    class MarkAndCompactCollector: public GC {
    public:
        MarkAndCompactCollector(): GC(new NaiveAllocator()) {}
        ~MarkAndCompactCollector() override {
            delete allocator;
        }
    };
}
