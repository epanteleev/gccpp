#include "Compact.h"
#include "gc/collectors/MarkAndCompactCollector.h"

namespace gccpp::details {
    void Compact::do_it(GC *_gc) {
        auto* gc = static_cast<gccpp::MarkAndCompactCollector*>(_gc);

        auto& allocated = allocator.alloc_stat();

        std::unordered_set<void*> new_addresses;
        for (auto& i: allocated) {
            ObjectPointer oop(static_cast<MarkWord*>(i));
            if (oop.mw()->color == MarkWord::Color::White) {
                allocator.free(i);
            } else {
                oop.mw()->color = MarkWord::Color::White;
                auto size = oop.mw()->size;

                auto new_addr = allocator.alloc(size);
                std::memcpy(new_addr, i, size);

                *reinterpret_cast<std::size_t*>(oop.mw()) = (size_t)(new_addr);
                new_addresses.insert(new_addr);
            }
        }

//        // todo update references.
//        for(auto i = 0; i < gc->shadowStack.size(); i++) {
//            worklist.push(gc->shadowStack[i]);
//        }
//
//        while (!worklist.empty()) {
//            auto top = worklist.top();
//            worklist.pop();
//            if (top == nullptr) {
//                continue;
//            }
//            if (top.mw()->color == MarkWord::Color::Black) {
//                continue;
//            }
//            top.mw()->color = MarkWord::Color::Black;
//
//            auto obj = top.content<GCCollected>();
//            obj->trace(this);
//        }
    }
}