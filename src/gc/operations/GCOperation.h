#pragma once

namespace gccpp {
    class MarkAndSweepCollector;
    class BasicCollector;

    namespace details {
        class ObjectPointer;
    }

    class GCOperation {
    public:
        virtual ~GCOperation() = default;
        virtual void trace(details::ObjectPointer& ptr) = 0;
        virtual void do_it(BasicCollector* gc) = 0;
    };
}
