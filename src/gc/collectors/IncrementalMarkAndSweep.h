#pragma once
#include "gc/collectors/BasicCollector.h"
#include "gc/containers/Buffer.h"
#include "gc/allocators/MallocBasedAllocator.h"
#include "gc/fwd.h"

namespace gccpp {
    class IncrementalMarkAndSweep : public BasicCollector {
        friend class details::MarkIncrementally;
    public:
        enum class Mode: std::size_t {
            NoGC,
            Marking,
            Sweeping
        };

    public:
        explicit IncrementalMarkAndSweep ():
                BasicCollector(new MallocBasedAllocator(2048)) {}

        ~IncrementalMarkAndSweep () override {
            delete allocator;
        }

    public:
        // Dijkstra barrier
        void write_barrier(details::ObjectPointer* des, const details::ObjectPointer& src) override {
            if (src == nullptr) {
                return;
            }
            *des = src;
            if (des->mw()->color() == MarkWord::Color::Black) {
                shade(src);
            }
        }

        void shade(const details::ObjectPointer& ptr) {
            if (ptr.mw()->color() == MarkWord::Color::Black) {
                return;
            }
            ptr.mw()->set_color(MarkWord::Color::Black);
            worklist.push(ptr);
        }

        void collect() override;
    private:
        Buffer<details::ObjectPointer> worklist;
        Mode mode{};
    };
}