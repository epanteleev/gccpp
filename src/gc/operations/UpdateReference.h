#pragma once

#include <stack>
#include "GCOperation.h"
#include "gc/containers/Buffer.h"
#include "pointer/ObjectPointer.h"

namespace gccpp::details {

    class UpdateReference final : public GCOperation {
    public:
        explicit UpdateReference(Buffer<details::ObjectPointer*>& _worklist):
            worklist(_worklist) {}

    public:
        void trace(details::ObjectPointer& ptr) override;
        std::size_t do_it(BasicCollector *gc) override;

    private:
        Buffer<details::ObjectPointer*>& worklist;
    };
}