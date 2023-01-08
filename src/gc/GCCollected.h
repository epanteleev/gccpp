#pragma once
#include "gc/operations/GCOperation.h"

namespace gccpp {
    class GCCollected {
    public:
        virtual void trace(GCOperation* visitor) noexcept = 0;
    };
}