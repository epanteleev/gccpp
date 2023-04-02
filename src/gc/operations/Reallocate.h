#pragma once

#include <stack>
#include "pointer/ObjectPointer.h"
#include "gc/operations/GCOperation.h"
#include "gc/allocators/MallocBasedAllocator.h"
#include "gc/containers/Buffer.h"

namespace gccpp::details {
    class Reallocate : public GCOperation {
    public:
        explicit Reallocate(Buffer<details::ObjectPointer*>& _worklist):
            worklist(_worklist) {}

        void trace(details::ObjectPointer& ptr) override {
            if (ptr == nullptr) {
                return;
            }
            worklist.push(&ptr);
        };
        std::size_t do_it(BasicCollector* gc) override;

    private:
        Buffer<details::ObjectPointer*>& worklist;
    };
}
