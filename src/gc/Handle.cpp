#include "gc/Handle.h"
#include "gc/containers/GlobalCtx.h"

namespace gccpp {

    HandleMark::HandleMark(ThreadCtx &threadCtx) :
        m_stack(threadCtx.ctx.stacks.stack_of_current_thread()) {
        saved_sp = m_stack.enter();
    }

    HandleMark::HandleMark(GlobalCtx &ctx) :
        m_stack(ctx.stacks.stack_of_current_thread()) {
        saved_sp = m_stack.enter();
    }
}