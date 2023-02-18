#include "Compact.h"
#include "gc/collectors/MarkAndCompactCollector.h"

#include <cassert>

namespace gccpp::details {
    void Compact::do_it(BasicCollector *gc) {
        auto allocator = dynamic_cast<SemispacesAllocator*>(gc->get_allocator());
        auto to_region = allocator->free_space;

        auto fn = [&](Header* header) {
            auto* mv = reinterpret_cast<MarkWord*>(reinterpret_cast<std::byte*>(header) + sizeof(Header));

            if (mv->color != MarkWord::Color::Black) {
                return;
            }
            auto new_address = to_region->alloc(header->chunk_size - sizeof(Header));
            assert(new_address != nullptr); //Todo
            mv->forwarding_pointer = new_address;
        };

        allocator->active_space->visit(fn);
    }
}