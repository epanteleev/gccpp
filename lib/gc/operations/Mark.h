#pragma once

#include <stack>

#include "gc/Gc.h"
#include "gc/GcCollected.h"
#include "gc/operations/GCOperation.h"

namespace tgc {
    class Mark : public GCOperation {
    public:
        Mark() = default;
        ~Mark() = default;

    public:
        void trace(details::ObjectPointer ptr) override {
            stack.push(ptr);
        }

        void do_it(tgc::Gc *gc) override;

    public:
        std::stack<details::ObjectPointer> stack;
    };
}