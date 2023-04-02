#pragma once

#include "gc/GarbageCollected.h"
#include "pointer/ObjectPointer.h"
#include "gc/operations/GCOperation.h"
#include "gc/collectors/IncrementalMarkAndSweep.h"

namespace gccpp::details {
    class MarkIncrementally: public GCOperation {
    public:
        MarkIncrementally(IncrementalMarkAndSweep* _gc):
            gc(_gc) {}
        ~MarkIncrementally() override = default;

    public:
        void trace(details::ObjectPointer& ptr) override;
        std::size_t do_it(BasicCollector *gc) override;
    private:
        IncrementalMarkAndSweep* gc;
    };
}
