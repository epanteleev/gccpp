#include "gc/containers/ThreadsStacks.h"
#include <cassert>
#include <mutex>

namespace gccpp::details {
    thread_local ShadowStack ThreadsStacks::my_stack{};

    ShadowStack& ThreadsStacks::stack_of_current_thread() {
        return my_stack;
    }

    void ThreadsStacks::initialize_for_current_thread() {
        std::lock_guard<SpinLock> _l(spinLock);
        auto id = std::this_thread::get_id();
        assert(!map.contains(id));

        map.emplace(id, &my_stack);
    }

    void ThreadsStacks::destroy_for_current_thread() {
        std::lock_guard<SpinLock> _l(spinLock);
        auto stack = map.find(std::this_thread::get_id());
        assert(stack != map.end());

        map.erase(stack);
    }

    void ThreadsStacks::visit(const std::function<void(element &)> &fn) noexcept {
        std::lock_guard<SpinLock> _l(spinLock);
        for (auto& item: map) {
            fn(item);
        }
    }
}