#pragma once
#include <deque>
#include <thread>

#include "SpinLock.h"

namespace gccpp::details {

    class ThreadSafepointState final {
    public:
        using element = std::pair<std::thread::id, long *>;
        using type = std::deque<element>;
        using iterator = type::iterator;

    public:
        void initialize_for_current_thread();
        void destroy_for_current_thread();

    private:
        iterator find_entry();

    private:
        static thread_local long self_suspend;

    private:
        SpinLock spinLock;
        type suspend;
    };
}