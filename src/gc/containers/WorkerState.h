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
            while (mode != Mode::Started) {
                if (mode == Mode::Collection) {
                    return false;
                }
                if (mode == Mode::Terminate) {
                    return true;
                }
                std::this_thread::yield();
            }
            return false;
        }

        bool wait_collect_command() {
            while (mode != Mode::Collection) {
                if (mode == Mode::Terminate) {
                    return true;
                }
                std::this_thread::yield();
            }
            return false;
        }

        void wait_collect_ending() {
            while (mode == Mode::Collection) {
                std::this_thread::yield();
            }
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
        }

        void start() {
            mode.store(Mode::Started);
        }

        void terminate() {
            mode.store(Mode::Terminate);
        }

    private:
        std::atomic <Mode> mode;
        WaitBarrier barrier;
    };

}
