#pragma once

#include <stack>

#include "gc/collectors/MarkAndSweepCollector.h"
#include "gc/GCCollected.h"
#include "gc/operations/GCOperation.h"

namespace gccpp::details {
    class Mark : public GCOperation {
    public:
        Mark() = default;
        ~Mark() override = default;

    public:
        void trace(details::ObjectPointer ptr) override {
            worklist.push(ptr);
        }

        void do_it(GC *gc) override;

    private:
        std::stack<details::ObjectPointer> worklist;
    };
}