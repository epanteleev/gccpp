#pragma once

#include "gc/GarbageCollected.h"
#include "pointer/Oop.inline.h"
#include "gc/containers/Enviroment.h"
#include "gc/operations/GCOperation.h"
#include "gc/collectors/MarkAndSweepCollector.h"

template<gccpp::GarbageCollectedType T, gccpp::GarbageCollectedType U>
class Pair : public gccpp::GarbageCollected {
public:
    Pair(gccpp::Oop<T> _first, gccpp::Oop<U> _second):
        first(_first),
        second(_second) {}

public:
    static gccpp::Oop<Pair> make(gccpp::Oop<T> _first, gccpp::Oop<U> _second) {
        return gccpp::Enviroment::context().alloc<Pair>(_first, _second);
    }

public:
    void trace(gccpp::GCOperation *operation) noexcept override {
        operation->trace(first);
        operation->trace(second);
    }

public:
    gccpp::Oop<T> first;
    gccpp::Oop<U> second;
};
