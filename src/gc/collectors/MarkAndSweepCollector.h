#pragma once

#include "gc/collectors/BasicCollector.h"
#include "gc/allocators/NaiveAllocator.h"
#include <cstring>
#include <cstdio>

namespace gccpp {
    namespace details {
        class Mark;
        class Sweep;
    }

    class MarkAndSweepCollector : public BasicCollector {
        friend class details::Mark;
        friend class details::Sweep;
    public:
        explicit MarkAndSweepCollector():
            BasicCollector(new NaiveAllocator()) {}

        ~MarkAndSweepCollector() override {
            delete allocator;
        }
    public:
        void collect() override;
    };
}