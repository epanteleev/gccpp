#pragma once

#include <stack>
#include "gc/GCCollected.h"
#include "pointer/ObjectPointer.h"
#include "gc/operations/GCOperation.h"

namespace gccpp::details {
    class Mark : public GCOperation {
    public:
        Mark() = default;
        ~Mark() override = default;

    public:
        void trace(details::ObjectPointer& ptr) override;
        void do_it(BasicCollector *gc) override;

    private:
        std::stack<details::ObjectPointer> worklist;
    };
}