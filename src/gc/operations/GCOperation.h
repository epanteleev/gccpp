#pragma once

#include "pointer/ObjectPointer.h"

namespace gccpp {
    class MarkAndSweepCollector;
    class GC;

    class GCOperation {
    public:
        virtual ~GCOperation() = default;
        virtual void trace(details::ObjectPointer ptr) = 0;
        virtual void do_it(GC* gc) = 0;
    };
}
