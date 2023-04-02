#pragma once

#include "gc/containers/Enviroment.h"
#include "gc/GarbageCollected.h"
#include "gc/collectors/MarkAndSweepCollector.h"
#include "gc/operations/GCOperation.h"
#include "pointer/Oop.inline.h"

class SizeT: public gccpp::GarbageCollected {
public:
    explicit SizeT(std::size_t _data):
        data(_data) {}
public:
    [[nodiscard]]
    std::size_t value() const {
        return data;
    }

public:
    void store(std::size_t newValue) {
        data = newValue;
    }
public:
    void trace(gccpp::GCOperation *operation) noexcept override {
        (void)(operation);
    }

public:
    static gccpp::Oop<SizeT> make(std::size_t data) {
        return gccpp::Enviroment::context().alloc<SizeT>(data);
    }

private:
    std::size_t data;
};
