#include "Reallocate.h"
#include "gc/collectors/MarkAndCompactCollector.h"

#include <cassert>

namespace gccpp::details {

    template<std::size_t SIZE>
    void compact(details::FixedSizeAllocator<SIZE>& allocator) {
        if (allocator.allocation_count() == 0) {
            return;
        }
        auto freed = allocator.left_finger();
        auto dirty = allocator.right_finger();

        if (Finger<SIZE>::crossed(freed, dirty)) {
            return;
        }
        while (!Finger<SIZE>::crossed(freed, dirty)) {
            std::memcpy(freed(), dirty(), SIZE);

            auto mv_dirty = reinterpret_cast<MarkWord*>(dirty());
            mv_dirty->set_forwarding_ptr(freed());
            freed = freed.next();
            dirty = dirty.next();
        }
        allocator.refresh_offset(dirty);
    }

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

        compact(allocator->small_allocator32);
        compact(allocator->small_allocator64);
        return 0;
    }
}