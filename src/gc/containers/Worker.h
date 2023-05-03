#pragma once

#include <thread>
#include <functional>
#include <utility>
#include "gc/fwd.h"

namespace gccpp::details {
    class Worker final {
    public:
        explicit Worker(gccpp::Environment& _ctx):
                ctx(_ctx) {}

        ~Worker() = default;
    public:
        Worker(Worker&) = delete;
        Worker operator=(Worker&) = delete;

    public:
        void stop();
        void start();

    private:
        static void run(Worker&) noexcept;
        bool wait_job();
    private:
        gccpp::Environment& ctx;
        std::thread worker{};
    };
}


