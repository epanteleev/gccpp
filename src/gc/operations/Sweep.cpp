#include "gc/operations/Sweep.h"
#include "gc/collectors/MarkAndSweepCollector.h"
#include <cassert>

namespace gccpp::details {
    void Sweep::do_it(BasicCollector *_gc) {
        auto gc = dynamic_cast<MarkAndSweepCollector*>(_gc);
        assert(gc != nullptr);

        auto& allocated = dynamic_cast<NaiveAllocator*>(gc->allocator)->alloc_stat();

        auto predicate = [&](void* addr) -> bool {
            ObjectPointer oop(static_cast<MarkWord*>(addr));
            if (oop.mw()->color == MarkWord::Color::White) {
                gc->allocator->free(addr);
                return true;
            } else {
                oop.mw()->color = MarkWord::Color::White;
                return false;
            }
        };
        std::erase_if(allocated, predicate);
    }
}