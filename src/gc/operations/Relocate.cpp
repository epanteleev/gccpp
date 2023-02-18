#include "gc/operations/Relocate.h"

#include "gc/allocators/SemispacesAllocator.h"
#include "gc/collectors/BasicCollector.h"
#include <cassert>

namespace gccpp::details {
    void Relocate::do_it(gccpp::BasicCollector *gc) {
        auto allocator = dynamic_cast<SemispacesAllocator*>(gc->get_allocator());
        assert(allocator != nullptr);

        auto fn = [&](Header* header) -> void {
            auto* mv = reinterpret_cast<MarkWord*>(reinterpret_cast<std::byte*>(header) + sizeof(Header));

            if (mv->color != MarkWord::Color::Black) {
                return;
            }
            auto new_address = static_cast<MarkWord*>(mv->forwarding_pointer);
            std::memcpy(new_address, mv, header->chunk_size - sizeof(Header));

            new_address->color = MarkWord::Color::White;
            new_address->forwarding_pointer = nullptr;
        };
        allocator->active_space->visit(fn);

        allocator->active_space->release();
        allocator->change_space();
    }
}
