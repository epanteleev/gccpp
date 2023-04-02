#include "gc/collectors/BasicCollector.h"
#include "gc/containers/Enviroment.h"

namespace gccpp {

    void BasicCollector::set_ctx(Enviroment *_ctx) {
        assert(ctx == nullptr);
        ctx = _ctx;
    }

}

