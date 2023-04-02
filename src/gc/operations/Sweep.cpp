#include "gc/operations/Sweep.h"
#include "gc/collectors/MarkAndSweepCollector.h"
#include <cassert>

namespace gccpp::details {
    std::size_t Sweep::do_it(BasicCollector *_gc) {
        auto gc = dynamic_cast<MarkAndSweepCollector*>(_gc);
        assert(gc != nullptr);

        auto allocator = dynamic_cast<MallocBasedAllocator*>(gc->allocator);
        assert(allocator != nullptr);

        auto predicate = [&](void* addr) -> bool {
            assert(addr != nullptr);
            ObjectPointer oop(static_cast<MarkWord*>(addr));
            if (oop.mw()->color == MarkWord::Color::White) {
                return true;
            } else {
                oop.mw()->color = MarkWord::Color::White;
                return false;
            }
        };
        allocator->free_if(predicate);
        return 0;
    }
}