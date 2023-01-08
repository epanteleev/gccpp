#pragma once

#include "gc/collectors/GC.h"
#include "gc/allocators/NaiveAllocator.h"
#include <cstring>
#include <cstdio>

namespace gccpp {
    namespace details {
        class Mark;
        class Sweep;
    }

    class MarkAndSweepCollector : public GC {
        friend class details::Mark;
        friend class details::Sweep;
    public:
        MarkAndSweepCollector(): GC(new NaiveAllocator()) {}
        ~MarkAndSweepCollector() override {
            delete allocator;
        }

    public:
        void safepoint_at_poll() override;
    };
}