#include "gc/operations/Mark.h"
#include "gc/collectors/BasicCollector.h"
#include <cassert>
#include "gc/containers/Environment.h"

namespace gccpp::details {
    std::size_t Mark::do_it(BasicCollector *gc) {
        auto& stacks = gc->context()->stacks;
        //Initial marking.
        auto visit = [&](ThreadsStacks::element& pair) {
            for(std::size_t i = 0; i < pair.second->size(); i++) {
                auto root = pair.second->addr(i);
                if (*root == nullptr) {
                    continue;
                }
                worklist.push(root);
            }
        };
        stacks.visit(visit);

        while (!worklist.empty()) {
            auto top = *worklist.pop();
            assert(top != nullptr);

            if (top.mw()->color() == MarkWord::Color::Black) {
                continue;
            }
            top.mw()->set_color(MarkWord::Color::Black);

            top.trace(this);
        }
        return 0;
    }

    void Mark::trace(ObjectPointer &ptr) {
        if (ptr == nullptr) {
            return;
        }
        worklist.push(&ptr);
    }

}