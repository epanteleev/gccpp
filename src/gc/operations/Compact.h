#pragma once

#include <stack>
#include "pointer/ObjectPointer.h"
#include "gc/operations/GCOperation.h"
#include "gc/allocators/NaiveAllocator.h"

namespace gccpp::details {
    class Compact : public GCOperation {
    public:
        Compact() = default;

        void trace(details::ObjectPointer& ptr) override {
            if (ptr == nullptr) {
                return;
            }
            worklist.push(&ptr);
        };
        void do_it(GC* gc) override;

    private:
        std::stack<details::ObjectPointer*> worklist;
    };
}
