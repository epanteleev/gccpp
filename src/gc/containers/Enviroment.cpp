#include "gc/containers/Enviroment.h"
#include "gc/containers/Memory.h"

#include <sstream>

namespace gccpp {

     std::unique_ptr<Enviroment> Enviroment::globalCtx{};

    Enviroment::Enviroment(std::unique_ptr<BasicCollector> _gc) :
        gc(std::move(_gc)),
        worker(*this) { //Todo
        worker.start();
    }

    void Enviroment::force_gc() {
        start_collection();
        safepoint();
    }

    void Enviroment::safepoint_slow() {
        global_lock.unlock();
        state.wait_collect_ending();
        global_lock.lock();

        assert(self_suspend == false);
    }

    void Enviroment::initialize_for_current_thread() {
        global_lock.initialize_for_current_thread();
        stacks.initialize_for_current_thread();
        global_lock.lock();
    }

    void Enviroment::destroy_for_current_thread() {
        global_lock.unlock();
        stacks.destroy_for_current_thread();
        global_lock.destroy_for_current_thread();
    }

    void Enviroment::start_collection() {
        self_suspend = true;
        details::mem::write_barrier();
        worker.collect();
    }

    void Enviroment::unmanaged_context(const std::function<void()>& closure) {
        global_lock.unlock();
        closure();
        global_lock.lock();
    }

    Enviroment& Enviroment::init(std::unique_ptr<BasicCollector> gc) {
        globalCtx.reset();
        globalCtx = std::make_unique<Enviroment>(std::move(gc));
        globalCtx->gc->set_ctx(globalCtx.get());
        return *globalCtx;
    }

    void* Enviroment::raw_alloc_helper(std::size_t actual_size, bool need_zeroing) {
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

    void Enviroment::oom_report() {
        std::ostringstream sstr;
        gc->allocator->print(sstr);
        std::fprintf(stderr, "Out of memory:\n%s", sstr.str().c_str());
        std::terminate();
    }

    ThreadEnv::ThreadEnv(Enviroment &_ctx): ctx(_ctx) {
        ctx.initialize_for_current_thread();
    }

    ThreadEnv::~ThreadEnv() {
        ctx.destroy_for_current_thread();
    }
}
