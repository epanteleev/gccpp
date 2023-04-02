#pragma once

#include "gc/collectors/MarkAndSweepCollector.h"
#include "gc/operations/GCOperation.h"
#include "gc/GarbageCollected.h"
#include "gc/allocators/MallocBasedAllocator.h"
#include "pointer/ObjectPointer.h"

namespace gccpp::details {
    class Sweep : public GCOperation {
    public:
        Sweep() = default;
        ~Sweep() override = default;

    public:
        void trace(details::ObjectPointer& ptr) override {
            (void)(ptr);
        }

        std::size_t do_it(BasicCollector *gc) override;
    };
}
