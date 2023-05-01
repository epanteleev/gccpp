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

//        auto begin = reinterpret_cast<std::size_t>(allocator->small_allocator32.start_ptr);
//        auto end = begin + allocator->small_allocator32.offset;
//        auto current = begin;
//
//        auto next_free_slot = [&](std::size_t current) {
//            auto object = reinterpret_cast<MarkWord*>(current);
//            while (object->color() != MarkWord::Color::White) {
//                current += 32;
//                object = reinterpret_cast<MarkWord*>(current);
//            }
//            return current;
//        };
//
//        auto next_dirty_slot = [&](std::size_t current) {
//            auto object = reinterpret_cast<MarkWord*>(current);
//            while (object->color() != MarkWord::Color::Black) {
//                current -= 32;
//                object = reinterpret_cast<MarkWord*>(current);
//            }
//            return current;
//        };
//
//        if (begin >= end) {
//            return 0;
//        }
//        auto freed = next_free_slot(current);
//        auto dirty = next_dirty_slot(end - 32);
//
//        while (freed < dirty) {
//            std::memcpy(reinterpret_cast<void*>(freed), reinterpret_cast<void*>(dirty), 32);
//
//            auto mv_dirty = reinterpret_cast<MarkWord*>(dirty);
//            mv_dirty->set_forwarding_ptr(reinterpret_cast<void*>(freed));
//            freed = next_free_slot(freed);
//            dirty = next_dirty_slot(dirty);
//        }
        return 0;
    }
}