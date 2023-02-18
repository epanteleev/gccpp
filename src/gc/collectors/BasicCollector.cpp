#include "gc/collectors/BasicCollector.h"
#include "gc/containers/GlobalCtx.h"

namespace gccpp {

    void BasicCollector::set_ctx(GlobalCtx *_ctx) {
        assert(ctx == nullptr);
        ctx = _ctx;
    }

    void BasicCollector::start_collection() {
        ctx->start_collection();
        ctx->poll_at_safepoint();
    }

    details::ObjectPointer *BasicCollector::push(const details::ObjectPointer &ptr) {
        return ctx->stacks.stack_of_current_thread().push(ptr);
    }
}

