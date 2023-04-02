#pragma once
#include <thread>
#include <functional>
#include <utility>
#include "WaitBarrier.h"

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
        bool wait_start() {
            barrier.wait([&] {
                return mode == Mode::Started || mode == Mode::Collection || mode == Mode::Terminate;
            });
            return false;
        }

        void wait_collect_command() {
            barrier.wait([&] {
                return mode == Mode::Collection || mode == Mode::Terminate;
            });
        }

        void wait_collect_ending() {
            barrier.wait([&] {
                return mode == Mode::Started;
            });
        }

        bool is_terminate() {
            return mode.load() == Mode::Terminate;
        }

        bool is_started() {
            return mode.load() == Mode::Started;
        }

        bool is_collect() {
            return mode.load() == Mode::Collection;
        }

        void collect() {
            mode.store(Mode::Collection);
            barrier.notify();
        }

        void start() {
            mode.store(Mode::Started);
            barrier.notify();
        }

        void mutators_continue() {
            mode.store(Mode::Started);
            barrier.notify();
        }

        void terminate() {
            mode.store(Mode::Terminate);
            barrier.notify();
        }

    private:
        std::atomic <Mode> mode;
        WaitBarrier barrier;
    };

}
