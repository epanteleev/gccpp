#include "gc/containers/GlobalCtx.h"

namespace gccpp {

    GlobalCtx::GlobalCtx(BasicCollector* _gc) :
            worker([&] { gc->collect(); }, this), //Todo
            gc(_gc) {
        worker.start();
    }

    void GlobalCtx::poll_at_safepoint() {
        start_collection(); // todo for debug purpose
        if (!self_suspend) {
            return;
        }
        poll_at_safepoint_slow();
    }

    void GlobalCtx::poll_at_safepoint_slow() {
        global_lock.unlock();
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(23));
        } while (is_collection());

        global_lock.lock();

        assert(self_suspend == false);
    }

    std::unique_ptr<GlobalCtx> GlobalCtx::initialize(BasicCollector *gc) {
        auto ctx = std::make_unique<GlobalCtx>(gc);
        ctx->gc->set_ctx(ctx.get()); // todo
        return ctx;
    }

    void GlobalCtx::initialize_for_current_thread() {
        global_lock.initialize_for_current_thread();
        stacks.initialize_for_current_thread();
        global_lock.lock();
    }

    void GlobalCtx::destroy_for_current_thread() {
        global_lock.unlock();
        stacks.destroy_for_current_thread();
        global_lock.destroy_for_current_thread();
    }

    void GlobalCtx::start_collection() {
        self_suspend = true;
        worker.collect();
    }

    bool GlobalCtx::is_collection() {
        return worker.is_collect();
    }

    void GlobalCtx::call_unmanaged(const std::function<void()>& closure) {
        global_lock.unlock();
        closure();
        global_lock.lock();
    }

    ThreadCtx::ThreadCtx(GlobalCtx &_ctx): ctx(_ctx) {
        ctx.initialize_for_current_thread();
    }

    ThreadCtx::~ThreadCtx() {
        ctx.destroy_for_current_thread();
    }
}
