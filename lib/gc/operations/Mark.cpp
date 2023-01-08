#include "gc/operations/Mark.h"
#include "gc/Gc.h"

namespace tgc {
    void Mark::do_it(Gc *gc) {
        //Initial marking.
        for(auto i = 0; i < gc->shadowStack.size(); i++) {
            stack.push(gc->shadowStack[i]);
        }

        while (!stack.empty()) {
            auto top = stack.top();
            stack.pop();
            if (top == nullptr) {
                continue;
            }
            if (top.markWord()->color == MarkWord::Color::Black) {
                continue;
            }
            top.markWord()->color = MarkWord::Color::Black;

            auto obj = top.content<GcCollected>();
            obj->trace(this);
        }
    }
}