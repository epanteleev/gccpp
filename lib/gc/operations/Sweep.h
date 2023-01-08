#pragma once

#include "gc/Gc.h"
#include "gc/operations/GCOperation.h"
#include "gc/GcCollected.h"
#include "gc/Allocator.h"
#include "pointer/ObjectPointer.h"

namespace tgc {
    class Sweep : public GCOperation {
    public:
        Sweep() = default;
        ~Sweep() = default;

    public:
        void trace(details::ObjectPointer ptr) override {
            if (ptr == nullptr) {
                return;
            }
            if (ptr.markWord()->color == MarkWord::Color::Black) {
                live_set.push_back(ptr.markWord());
            }
            ptr.markWord()->color = MarkWord::Color::White;
        }

        void do_it(Gc *gc) override;
    public:
        std::vector<void*> live_set;
    };
}
