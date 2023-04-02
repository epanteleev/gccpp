#pragma once
#include "gc/containers/Worker.h"
#include "gc/collectors/BasicCollector.h"
#include "gc/containers/ThreadLock.h"
#include "gc/fwd.h"
#include "WorkerState.h"
#include "Memory.h"

namespace gccpp {

    class ThreadEnv final {
        friend class HandleMark;
    public:
        explicit ThreadEnv(Enviroment& ctx);
        ~ThreadEnv();
    private:
        Enviroment& ctx;
    };

    class Enviroment final {
        friend class BasicCollector;
        friend class details::Worker;
        friend class ThreadEnv;
        friend class HandleMark;
        friend class details::Mark;
        friend class details::UpdateReference;
        friend class details::MarkIncrementally;

        template<typename T>
        friend class Handle;
    public:
        explicit Enviroment(std::unique_ptr<BasicCollector> _gc);

        ~Enviroment() {
            worker.stop();
        }
    public:
        template<gccpp::GarbageCollectedType T, typename... Args>
        static inline gccpp::Oop<T> init_object(gccpp::Oop<T> oop, Args... args) {
            new(oop.mw()->object()) T(std::forward<Args>(args)...);
            return oop;
        }

    public:
        void force_gc();

        inline void safepoint() {
            if (!self_suspend) {
                return;
            }
            details::mem::write_barrier();
            safepoint_slow();
        }

        void unmanaged_context(const std::function<void()>& closure);

        template<GarbageCollectedType T>
        inline Oop<T> raw_alloc(std::size_t additional_bytes_count, bool need_zeroing = true) {
            std::size_t size = sizeof(MarkWord) + sizeof(T) + additional_bytes_count;
            auto *ptr = raw_alloc_helper(size, need_zeroing);

            return Oop<T>(reinterpret_cast<MarkWord *>(ptr));
        }

        template<GarbageCollectedType T, typename... Args>
        inline Oop<T> alloc(Args &&... args) {
            auto oop = raw_alloc<T>(0);
            new(oop.mw()->object()) T(std::forward<Args>(args)...);
            return oop;
        }

    private:
        void initialize_for_current_thread();
        void destroy_for_current_thread();

    private:
        void start_collection();
        void safepoint_slow();
        void* raw_alloc_helper(std::size_t actual_size, bool need_zeroing);
        void oom_report();

    public:
        static Enviroment& init(std::unique_ptr<BasicCollector> gc);
        inline static Enviroment& context() {
            return *globalCtx;
        }
        inline static BasicCollector* collector() {
            return globalCtx->gc.get();
        }

    private:
        static std::unique_ptr<Enviroment> globalCtx;

    private:
        std::unique_ptr<BasicCollector> gc{};
        details::Worker worker;
        details::WorkerState state;
        details::ThreadLock global_lock;
        volatile bool self_suspend{};
        details::ThreadsStacks stacks;
    };
}

