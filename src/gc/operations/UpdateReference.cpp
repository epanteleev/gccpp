
#include <cassert>
#include "UpdateReference.h"
#include "gc/collectors/BasicCollector.h"
#include "gc/containers/Environment.h"

namespace gccpp::details {
    std::size_t UpdateReference::do_it(BasicCollector *gc) {
        auto& stacks = gc->context()->stacks;

        stacks.visit([&](ThreadsStacks::element& pair) {
            for(std::size_t i = 0; i < pair.second->size(); i++) {
                auto root = pair.second->addr(i);
                if (*root == nullptr) {
                    continue;
                }
                process_pointer(root);
            }
        });

        while (!worklist.empty()) {
            auto top = worklist.pop();
            if (*top == nullptr) {
                continue;
            }
            if (top->mw()->color() != MarkWord::Color::Black) {
                continue;
            }
            process_pointer(top);
        }
        return 0;
    }

    void UpdateReference::trace(ObjectPointer &ptr) {
        if (ptr == nullptr) {
            return;
        }
        worklist.push(&ptr);
    }

    void UpdateReference::process_pointer(ObjectPointer* oop) noexcept {
        oop->trace(this);

        if (oop->mw()->forwarding_ptr() == nullptr) {
            oop->mw()->set_color(MarkWord::Color::White);
            return;
        }
        oop->update(oop->mw()->forwarding_ptr());
        oop->mw()->set_color(MarkWord::Color::White);
    }
}

