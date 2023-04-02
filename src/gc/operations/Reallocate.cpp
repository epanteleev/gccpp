#include "Reallocate.h"
#include "gc/collectors/MarkAndCompactCollector.h"

#include <cassert>

namespace gccpp::details {
    std::size_t Reallocate::do_it(BasicCollector *gc) {
        auto allocator = dynamic_cast<SemispacesAllocator*>(gc->allocator);
        auto to_region = allocator->free_space;

        auto fn = [&](Chunk* header) {
            auto* mv = header->mw();

            if (mv->color() != MarkWord::Color::Black) {
                return;
            }
            auto new_address = to_region->alloc(header->object_size());
            assert(new_address != nullptr);
            assert(mv->forwarding_ptr() == nullptr);
            mv->set_forwarding_ptr(new_address);
        };

        allocator->active_space->visit(fn);
        return 0;
    }
}