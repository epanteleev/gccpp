#pragma once

namespace gccpp {
    class GCOperation;

    class GCCollected {
    public:
        virtual void trace(GCOperation* operation) noexcept = 0;
    };
}