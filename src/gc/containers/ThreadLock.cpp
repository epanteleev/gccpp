#include "ThreadLock.h"
#include <mutex>
#include <cassert>

namespace gccpp::details {
    void ThreadLock::initialize_for_current_thread() {
        std::unique_lock _l(mutex);
        auto id = std::this_thread::get_id();
        assert(!lock_map.contains(id));

        lock_map.emplace(id, std::make_unique<std::mutex>());
    }

    void ThreadLock::destroy_for_current_thread() {
        std::unique_lock _l(mutex);
        auto mut_ptr = find_entry();

        lock_map.erase(mut_ptr);
    }

    void ThreadLock::lock() {
        std::shared_lock _l(mutex);
        auto mut_ptr = find_entry();
        mut_ptr->second->lock();
    }

    void ThreadLock::unlock() {
        std::shared_lock _l(mutex);
        auto mut_ptr = find_entry();
        mut_ptr->second->unlock();
    }

    void ThreadLock::run_world() {
        std::shared_lock _l(mutex);
        for (auto &[_, mut]: lock_map) {
            mut->unlock();
        }
    }

    bool ThreadLock::try_stw() {
        std::shared_lock _l(mutex);
        for (auto &[_, mut]: lock_map) {
            mut->lock();
        }
        return true;
    }

    ThreadLock::type::iterator ThreadLock::find_entry() {
        auto mut_ptr = lock_map.find(std::this_thread::get_id());
        assert(mut_ptr != lock_map.end());
        return  mut_ptr;
    }
}