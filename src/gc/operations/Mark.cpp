#include "gc/operations/Mark.h"
#include "gc/collectors/BasicCollector.h"
#include <cassert>
#include "gc/containers/Environment.h"

namespace gccpp::details {
    std::size_t Mark::do_it(BasicCollector *gc) {
        auto& stacks = gc->context()->stacks;
        //Initial marking.
        auto visit = [&](ThreadsStacks::element& pair) -> void {
            for(std::size_t i = 0; i < pair.second->size(); i++) {
                auto root = pair.second->addr(i);
                if (*root == nullptr) {
                    continue;
                }
                process_pointer(root);
            }
        };
        stacks.visit(visit);

        while (!worklist.empty()) {
            auto top = worklist.pop();
            assert(*top != nullptr);

            process_pointer(top);
        }
        return 0;
    }

    void Mark::trace(ObjectPointer &ptr) {
        if (ptr == nullptr) {
            return;
        }
        worklist.push(&ptr);
    }

    void Mark::process_pointer(ObjectPointer *oop) noexcept {
        if (oop->mw()->color() == MarkWord::Color::Black) {
            return;
        }
        oop->mw()->set_color(MarkWord::Color::Black);
        oop->trace(this);
    }

}