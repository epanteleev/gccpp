#pragma once

#include <thread>
#include <shared_mutex>
#include <unordered_map>

namespace gccpp::details {
    class ThreadLock final {
    public:
        using type = std::unordered_map<std::thread::id, std::unique_ptr<std::mutex>>; //todo pointer???
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
        bool try_stw();
        void run_world();

    private:
        type::iterator find_entry();
    private:
        type lock_map;
        mutable std::shared_mutex mutex;
    };

}