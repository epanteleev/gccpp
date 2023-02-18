#pragma once

#include <thread>
#include <functional>
#include <utility>
#include <condition_variable>

namespace gccpp {
    class BasicCollector;
    class GlobalCtx;
}

namespace gccpp::details {

    class Worker final {
        friend class GlobalCtx;
    public:
        explicit Worker(std::function<void()> _task, gccpp::GlobalCtx* _ctx):
            ctx(_ctx),
            periodic_task(std::move(_task)),
            worker(run, this) {}

    public:
        void stop() {
            mode.store(2);
            worker.join();
        }

        void start() {
            mode.store(1);
        }

        void collect() {
            mode.store(3);
        }
        bool is_terminate();
        bool is_started();
        bool is_collect();
    private:
        static void run(Worker*) noexcept;

    private:
        std::thread worker;
        std::atomic<int> mode{}; //todo
        gccpp::GlobalCtx* ctx;
        std::function<void()> periodic_task;
    };
}


