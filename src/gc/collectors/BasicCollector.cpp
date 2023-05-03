#include "gc/collectors/BasicCollector.h"
#include "gc/containers/Environment.h"

namespace gccpp {

    void BasicCollector::set_ctx(Environment *_ctx) {
        assert(ctx == nullptr);
        ctx = _ctx;
    }

}

