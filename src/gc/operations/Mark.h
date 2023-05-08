#pragma once

#include <vector>
#include "gc/GarbageCollected.h"
#include "pointer/ObjectPointer.h"
#include "gc/operations/GCOperation.h"
#include "gc/containers/Buffer.h"

namespace gccpp::details {
    class Mark : public GCOperation {
    public:
        explicit Mark(Buffer<details::ObjectPointer*>& _worklist):
            worklist(_worklist) {}

        ~Mark() override = default;

    public:
        void trace(details::ObjectPointer& ptr) override;
        std::size_t do_it(BasicCollector *gc) override;

    private:
        void process_pointer(ObjectPointer* oop) noexcept;

    private:
        Buffer<details::ObjectPointer*>& worklist;
    };
}