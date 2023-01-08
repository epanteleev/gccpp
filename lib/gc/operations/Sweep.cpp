#include "gc/operations/Sweep.h"
#include "gc/Gc.h"

namespace tgc {
    void Sweep::do_it(Gc *gc) {
        for(auto i = 0; i < gc->shadowStack.size(); i++) {
            trace(gc->shadowStack[i]);
            auto obj = gc->shadowStack[i].content<GcCollected>();
            obj->trace(this);
        }

        gc->allocator.free_dead(std::move(live_set));
    }
}