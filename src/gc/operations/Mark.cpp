#include "gc/operations/Mark.h"
#include "gc/collectors/BasicCollector.h"
#include <cassert>
#include "gc/containers/GlobalCtx.h"

namespace gccpp::details {
    void Mark::do_it(BasicCollector *gc) {
        auto& stacks = gc->context()->all_stacks();
        //Initial marking.
        for(auto&[_, stack]: stacks) {
            for(std::size_t i = 0; i < stack.size(); i++) {
                worklist.push(stack[i]);
            }
        }

        while (!worklist.empty()) {
            auto top = worklist.top();
            worklist.pop();
            if (top == nullptr) {
                continue;
            }
            if (top.mw()->color == MarkWord::Color::Black) {
                continue;
            }
            top.mw()->color = MarkWord::Color::Black;

            top.trace(this);
        }
    }

    void Mark::trace(ObjectPointer &ptr) {
        if (ptr == nullptr) {
            return;
        }
        worklist.push(ptr);
    }
}