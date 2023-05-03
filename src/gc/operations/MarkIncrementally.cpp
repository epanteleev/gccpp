#include "MarkIncrementally.h"
#include "gc/containers/Environment.h"
#include <ctime>

namespace gccpp::details {
    void MarkIncrementally::trace(gccpp::details::ObjectPointer &ptr) {
        if (ptr == nullptr) {
            return;
        }
        if (ptr.mw()->color() == MarkWord::Color::Black) {
            return;
        }
        gc->worklist.push(ptr);
    }

    std::size_t MarkIncrementally::do_it(BasicCollector *_gc) {
        (void)(_gc);
        auto& stacks = gc->context()->stacks;
        //Initial marking.
        auto visit = [&](ThreadsStacks::element& pair) {
            for(std::size_t i = 0; i < pair.second->size(); i++) {
                if (pair.second->at(i) == nullptr) {
                    continue;
                }
                gc->worklist.push(pair.second->at(i));
            }
        };
        stacks.visit(visit);

        while (!gc->worklist.empty()) {
            auto top = gc->worklist.pop();

            if (top.mw()->color() == MarkWord::Color::Black) {
                continue;
            }
            top.mw()->set_color(MarkWord::Color::Black);

            top.trace(this);
        }
        return 0;
    }
}

