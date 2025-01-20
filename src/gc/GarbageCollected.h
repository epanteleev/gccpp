#pragma once

#include <concepts>

namespace gccpp {
    class GCOperation;

    class GarbageCollected {
    public:
        virtual ~GarbageCollected() = default;
        virtual void trace(GCOperation* operation) noexcept = 0;
    };

    template<typename T>
    concept GarbageCollectedType = std::derived_from<T, GarbageCollected>;
}