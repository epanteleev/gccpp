#pragma once


#include <stack>

#include "gc/GCCollected.h"
#include "gc/operations/GCOperation.h"

namespace gccpp::details {
    class Relocate : public GCOperation {
    public:
        Relocate() = default;

    public:
        void trace(details::ObjectPointer& ptr) override {}

        void do_it(GC *gc) override;
    };
}