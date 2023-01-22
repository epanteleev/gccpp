#pragma once

#include "gc/collectors/MarkAndSweepCollector.h"
#include "gc/operations/GCOperation.h"
#include "gc/GCCollected.h"
#include "gc/allocators/NaiveAllocator.h"
#include "pointer/ObjectPointer.h"

namespace gccpp::details {
    class Sweep : public GCOperation {
    public:
        Sweep() = default;
        ~Sweep() override = default;

    public:
        void trace(details::ObjectPointer& ptr) override {}

        void do_it(GC *gc) override;
    };
}
