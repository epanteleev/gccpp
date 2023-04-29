#include <algorithm>
#include <cassert>
#include "ThreadSafepointState.h"

namespace gccpp::details {
    thread_local long ThreadSafepointState::self_suspend{};

    void ThreadSafepointState::initialize_for_current_thread() {
        std::lock_guard<SpinLock> _l(spinLock);
        auto id = std::this_thread::get_id();
        suspend.emplace_back(id, &self_suspend);
    }

    void ThreadSafepointState::destroy_for_current_thread() {
        std::lock_guard<SpinLock> _l(spinLock);
        auto mut_ptr = find_entry();
        suspend.erase(mut_ptr);
    }

    ThreadSafepointState::iterator ThreadSafepointState::find_entry() {
        auto current_thread = std::this_thread::get_id();
        auto pred = [&](element & pair) {
            return pair.first == current_thread;
        };

        auto pair = std::find_if(suspend.begin(), suspend.end(), pred);
        assert(pair != suspend.end());
        return pair;
    }
}