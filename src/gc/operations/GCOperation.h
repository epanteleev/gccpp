#pragma once
#include "gc/fwd.h"

namespace gccpp {

    class GCOperation {
    public:
        virtual ~GCOperation() = default;
        virtual void trace(details::ObjectPointer& ptr) = 0;
        virtual std::size_t do_it(BasicCollector* gc) = 0;
    };
}