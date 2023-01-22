
#include <cassert>
#include "UpdateReference.h"
#include "gc/GCCollected.h"
#include "gc/operations/GCOperation.h"
#include "gc/collectors/GC.h"

namespace gccpp::details {
    void UpdateReference::do_it(GC *gc) {
        auto& stack = gc->stack();

        for(std::size_t i = 0; i < stack.size(); i++) {
            if (stack[i] == nullptr) {
                continue;
            }
            worklist.push(stack.addr(i));
        }

        while (!worklist.empty()) {
            auto top = worklist.top();
            worklist.pop();
            if (*top == nullptr) {
                continue;
            }
            if (top->mw()->color != MarkWord::Color::Black) {
                continue;
            }
            top->trace(this);

            assert(top->mw()->forwarding_pointer != nullptr);
            top->update( top->mw()->forwarding_pointer);
        }
    }

    void UpdateReference::trace(ObjectPointer &ptr) {
        if (ptr == nullptr) {
            return;
        }
        worklist.push(&ptr);
    }
}

