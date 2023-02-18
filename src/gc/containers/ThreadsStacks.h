#pragma once

#include "gc/containers/ShadowStack.h"

#include <shared_mutex>
#include <unordered_map>
#include <thread>

namespace gccpp::details {
    class ThreadsStacks final {
    public:
        using type = std::unordered_map<std::thread::id, ShadowStack>;
        using iterator = type::iterator;
    public:
        ShadowStack& stack_of_current_thread();

        void initialize_for_current_thread();
        void destroy_for_current_thread();
    public:
        iterator begin() {
            return map.begin();
        }

        iterator end() {
            return map.end();
        }
    private:
        std::shared_mutex mutex;
        type map;
    };
}