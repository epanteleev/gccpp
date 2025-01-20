#include "gc/containers/Environment.h"
#include "gc/containers/Memory.h"

#include <sstream>

namespace gccpp {

     std::unique_ptr<Environment> Environment::globalCtx{};

    Environment::Environment(std::unique_ptr<BasicCollector> _gc) :
        gc(std::move(_gc)),
        worker(*this) {}

    void Environment::force_gc() {
        start_collection();
        safepoint_slow();
    }

    void Environment::safepoint_slow() {
        thread_lock.unlock();
        state.wait_collect_ending();
        thread_lock.lock();
    }

    void Environment::initialize_for_current_thread() {
        {
            std::lock_guard _lock(env_lock);
            thread_lock.initialize_for_current_thread();
            stacks.initialize_for_current_thread();
            thread_lock.lock();
        }
        safepoint();
    }

    void Environment::destroy_for_current_thread() {
        std::lock_guard _lock(env_lock);
        thread_lock.unlock();
        stacks.destroy_for_current_thread();
        thread_lock.destroy_for_current_thread();
    }

    void Environment::start_collection() {
        self_suspend = true;
        details::mem::write_barrier();
        state.collect();
    }

    void Environment::unmanaged_context(const std::function<void()>& closure) {
        thread_lock.unlock();
        closure();
        thread_lock.lock();
    }

    Environment& Environment::init(std::unique_ptr<BasicCollector> gc) {
        globalCtx.reset();
        globalCtx = std::make_unique<Environment>(std::move(gc));
        globalCtx->gc->set_ctx(globalCtx.get());
        globalCtx->worker.start();
        return *globalCtx;
    }

    void* Environment::raw_alloc_helper(std::size_t actual_size, bool need_zeroing) {
        auto ptr = static_cast<std::byte *>(gc->allocator->alloc(actual_size));
        if (ptr == nullptr) {
            force_gc();
            ptr = static_cast<std::byte *>(gc->allocator->alloc(actual_size));
            if (ptr == nullptr) {
                oom_report();
            }
        }

        if (need_zeroing) {
            std::memset(ptr, 0, actual_size);
        } else {
            std::memset(ptr, 0, sizeof(MarkWord));
        }
        return ptr;
    }

    void Environment::oom_report() const {
        std::ostringstream sstr;
        gc->allocator->print(sstr);
        std::fprintf(stderr, "Out of memory:\n%s", sstr.str().c_str());
        std::terminate();
    }

    Environment::~Environment() {
        worker.stop();
        recorder.report(stderr);
    }

    ThreadEnv::ThreadEnv(Environment &_ctx): ctx(_ctx) {
        ctx.initialize_for_current_thread();
    }

    ThreadEnv::~ThreadEnv() {
        ctx.destroy_for_current_thread();
    }
}