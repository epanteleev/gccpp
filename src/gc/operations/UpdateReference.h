#pragma once

#include <stack>
#include "GCOperation.h"
#include "pointer/ObjectPointer.h"

namespace gccpp::details {

    class UpdateReference final : public GCOperation {
    public:
        UpdateReference() = default;

    public:
        void trace(details::ObjectPointer& ptr) override;
        void do_it(BasicCollector *gc) override;

    private:
        std::stack<details::ObjectPointer*> worklist;
    };
}