#pragma once
#include "gc/containers/Worker.h"
#include "gc/collectors/BasicCollector.h"
#include "gc/containers/ThreadLock.h"

namespace gccpp {

    class GlobalCtx;
    class HandleMark;

    class ThreadCtx final {
        friend class HandleMark;
    public:
        explicit ThreadCtx(GlobalCtx& ctx);
        ~ThreadCtx();
    private:
        GlobalCtx& ctx;
    };

    class GlobalCtx final {
        friend class BasicCollector;
        friend class details::Worker;
        friend class ThreadCtx;
        friend class HandleMark;
    public:
        explicit GlobalCtx(BasicCollector* _gc);

        ~GlobalCtx() {
            worker.stop();
        }
    public:
        void poll_at_safepoint();

        details::ThreadsStacks& all_stacks() noexcept {
            return stacks;
        }

        void call_unmanaged(const std::function<void()>& closure);

    private:
        void initialize_for_current_thread();
        void destroy_for_current_thread();

    private:
        void start_collection();
        void poll_at_safepoint_slow();
        bool is_collection();

    public:
        static std::unique_ptr<GlobalCtx> initialize(BasicCollector* gc);

    private:
        BasicCollector* gc{};
        details::Worker worker;
        details::ThreadLock global_lock;
        volatile bool self_suspend{};
        details::ThreadsStacks stacks;
    };
}

