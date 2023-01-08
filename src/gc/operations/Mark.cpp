#include "gc/operations/Mark.h"
#include "gc/collectors/MarkAndSweepCollector.h"
#include <cassert>

namespace gccpp::details {
    void Mark::do_it(GC *_gc) {
        auto gc = dynamic_cast<MarkAndSweepCollector*>(_gc);
        assert(gc != nullptr);

        //Initial marking.
        for(std::size_t i = 0; i < gc->shadowStack.size(); i++) {
            worklist.push(gc->shadowStack[i]);
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

            auto obj = top.content<GCCollected>();
            obj->trace(this);
        }
    }
}