#pragma once
#include <thread>
#include <functional>
#include <utility>
#include "WaitBarrier.h"
#include "Memory.h"

namespace gccpp::details {

    class WorkerState {
    public:
        enum class Mode : std::uint8_t {
            NotStarted,
            Started,
            Collection,
            Terminate
        };

    public:
        void wait_collect_command() {
            barrier.wait([&] {
                return is_terminate() || is_collect();
            });
        }

        void wait_collect_ending() {
            barrier.wait([&] {
                return mode == Mode::Started;
            });
        }

        bool is_terminate() {
            return mode == Mode::Terminate;
        }

        bool is_started() {
            return mode == Mode::Started;
        }

        bool is_collect() {
            return mode == Mode::Collection;
        }

        void collect() {
            mode = Mode::Collection;
            barrier.notify();
        }

        void start() {
            mode = Mode::Started;
            barrier.notify();
        }

        void mutators_continue() {
            start();
        }

        void terminate() {
            mode = Mode::Terminate;
            barrier.notify();
        }

    private:
        std::atomic<Mode> mode;
        WaitBarrier barrier;
    };

}
