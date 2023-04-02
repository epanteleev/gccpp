#pragma once


#include <stack>

#include "gc/GarbageCollected.h"
#include "gc/operations/GCOperation.h"

namespace gccpp::details {
    class Relocate : public GCOperation {
    public:
        Relocate() = default;

    public:
        void trace(details::ObjectPointer& ptr) override {
            (void)(ptr); //todo
        }

        std::size_t do_it(BasicCollector *gc) override;
    };
}