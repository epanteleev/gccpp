#pragma once
#include "gc/operations/GCOperation.h"

namespace tgc {
    class GcCollected {
    public:
        virtual void trace(GCOperation* visitor) noexcept = 0;
    };
}