#pragma once

#include "pointer/ObjectPointer.h"

namespace tgc {
    class Gc;

    class GCOperation {
    public:
        virtual void trace(details::ObjectPointer ptr) = 0;
        virtual void do_it(Gc* gc) = 0;
    };
}
