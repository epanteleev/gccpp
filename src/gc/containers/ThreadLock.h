#pragma once

#include <thread>
#include <shared_mutex>
#include <deque>

#include "gc/containers/SpinLock.h"

namespace gccpp::details {
    class ThreadLock final {
    public:
        using element = std::pair<std::thread::id, details::SpinLock*>;
        using type = std::deque<element>;
        using iterator = type::iterator;
    public:
        ThreadLock() = default;
        ~ThreadLock() = default;

    public:
        void initialize_for_current_thread();
        void destroy_for_current_thread();

    public:
        void lock();
        void unlock();

    public: // can be used only in gc threads
        bool stw();
        void run_world();

        iterator find_entry();
    private:
        static thread_local SpinLock threadLock;

    private:
        type lock_map;
        mutable SpinLock mutex;
    };
}