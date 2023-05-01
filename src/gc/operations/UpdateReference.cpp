
#include <cassert>
#include "UpdateReference.h"
#include "gc/collectors/BasicCollector.h"
#include "gc/containers/Enviroment.h"

namespace gccpp::details {
    std::size_t UpdateReference::do_it(BasicCollector *gc) {
        auto& stacks = gc->context()->stacks;

        stacks.visit([&](ThreadsStacks::element& pair) {
            for(std::size_t i = 0; i < pair.second->size(); i++) {
                auto root = pair.second->addr(i);
                if (*root == nullptr) {
                    continue;
                }
                worklist.push(root);
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
            top->mw()->set_color(MarkWord::Color::White);
            top->trace(this);

            if (top->mw()->forwarding_ptr() == nullptr) {
                continue;
            }
            top->update(top->mw()->forwarding_ptr());
        }
        return 0;
    }

    void UpdateReference::trace(ObjectPointer &ptr) {
        if (ptr == nullptr) {
            return;
        }
        worklist.push(&ptr);
    }
}

