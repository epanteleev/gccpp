#include "gc/operations/Relocate.h"

#include "gc/allocators/SemispacesAllocator.h"
#include "gc/collectors/BasicCollector.h"
#include <cassert>

namespace gccpp::details {
    std::size_t Relocate::do_it(gccpp::BasicCollector *gc) {
        auto allocator = dynamic_cast<SemispacesAllocator*>(gc->allocator);
        assert(allocator != nullptr);

        auto fn = [&](Chunk* header) -> void {
            auto* mw = header->mw();

            if (mw->color != MarkWord::Color::Black) {
                return;
            }
            auto new_address = static_cast<MarkWord*>(mw->forwarding_pointer);
            assert(new_address != nullptr);

            std::memcpy(new_address, mw, header->object_size());

            new_address->color = MarkWord::Color::White;
            new_address->forwarding_pointer = nullptr;
        };
        allocator->active_space->visit(fn);

        allocator->active_space->release();
        allocator->change_space();
        return 0;
    }
}
