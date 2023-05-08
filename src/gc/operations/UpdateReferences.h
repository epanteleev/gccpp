#pragma once

#include <stack>
#include "GCOperation.h"
#include "gc/containers/Buffer.h"
#include "pointer/ObjectPointer.h"

namespace gccpp::details {

    class UpdateReferences final : public GCOperation {
    public:
        explicit UpdateReferences(Buffer<details::ObjectPointer*>& _worklist):
            worklist(_worklist) {}

    public:
        void trace(details::ObjectPointer& ptr) override;
        std::size_t do_it(BasicCollector *gc) override;

    private:
        void process_pointer(ObjectPointer* oop) noexcept;

    private:
        Buffer<details::ObjectPointer*>& worklist;
    };
}