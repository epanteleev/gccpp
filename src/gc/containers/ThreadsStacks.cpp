#include "gc/containers/ThreadsStacks.h"
#include <cassert>
#include <mutex>

namespace gccpp::details {
    ShadowStack& ThreadsStacks::stack_of_current_thread() {
        std::shared_lock lock(mutex);
        auto stack = map.find(std::this_thread::get_id());
        assert(stack != map.end());
        return stack->second;
    }

    void ThreadsStacks::initialize_for_current_thread() {
        std::unique_lock lock(mutex);
        auto id = std::this_thread::get_id();
        assert(!map.contains(id));

        map.emplace(id, ShadowStack());
    }

    void ThreadsStacks::destroy_for_current_thread() {
        std::unique_lock lock(mutex);
        auto stack = map.find(std::this_thread::get_id());
        assert(stack != map.end());

        map.erase(stack);
    }
}