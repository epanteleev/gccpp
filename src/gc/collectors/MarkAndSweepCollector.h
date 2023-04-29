#pragma once

#include "gc/collectors/BasicCollector.h"
#include "gc/allocators/MallocBasedAllocator.h"
#include "gc/containers/Buffer.h"
#include <cstring>
#include <cstdio>

namespace gccpp {

    class MarkAndSweepCollector : public BasicCollector {
        friend class details::Mark;
        friend class details::Sweep;
    public:
        explicit MarkAndSweepCollector(std::size_t max_size):
            BasicCollector(new MallocBasedAllocator(max_size)) {}

        ~MarkAndSweepCollector() override {
            delete allocator;
        }

    public:
        void collect() override;

    private:
        details::Buffer<details::ObjectPointer*> worklist{};
    };
}