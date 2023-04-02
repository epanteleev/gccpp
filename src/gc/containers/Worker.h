#pragma once

#include <thread>
#include <functional>
#include <utility>
#include "gc/fwd.h"

namespace gccpp::details {
    class Worker final {
    public:
        explicit Worker(gccpp::Enviroment& _ctx):
                ctx(_ctx),
                worker(run, std::ref(*this)) {}

        ~Worker() = default;
    public:
        Worker(Worker&) = delete;
        Worker operator=(Worker&) = delete;

    public:
        void stop();
        void start();
        void collect();

    private:
        static void run(Worker&) noexcept;
        bool wait_job();
    private:
        gccpp::Enviroment& ctx;
        std::thread worker;
    };
}


