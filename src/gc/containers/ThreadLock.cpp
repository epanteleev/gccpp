#include "ThreadLock.h"
#include <mutex>
#include <cassert>
#include <deque>
#include <functional>

namespace gccpp::details {
     thread_local SpinLock ThreadLock::threadLock = SpinLock();

    void ThreadLock::initialize_for_current_thread() {
        std::lock_guard<SpinLock> _lock(mutex);
        auto id = std::this_thread::get_id();

        lock_map.emplace_back(id, &threadLock);
    }

    void ThreadLock::destroy_for_current_thread() {
        std::lock_guard<SpinLock> _lock(mutex);
        auto mut_ptr = find_entry();

        lock_map.erase(mut_ptr);
    }

    void ThreadLock::lock() {
        threadLock.lock();
    }

    void ThreadLock::unlock() {
        threadLock.unlock();
    }

    void ThreadLock::run_world() {
        std::lock_guard<SpinLock> _lock(mutex);
        for (auto &[_, mut]: lock_map) {
            mut->unlock();
        }
    }

    bool ThreadLock::stw() {
        std::lock_guard<SpinLock> _lock(mutex);
        for (auto &[_, mut]: lock_map) {
            mut->lock();
        }
        return true;
    }

    ThreadLock::iterator ThreadLock::find_entry() {
        auto current_thread = std::this_thread::get_id();
        auto pred = [&](element & pair) {
            return pair.first == current_thread;
        };

        auto pair = std::find_if(lock_map.begin(), lock_map.end(), pred);
        assert(pair != lock_map.end());
        return pair;
    }
}
