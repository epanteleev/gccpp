#pragma once

#include "gc/containers/ShadowStack.h"
#include "SpinLock.h"

#include <functional>
#include <unordered_map>
#include <thread>

namespace gccpp::details {
    class ThreadsStacks final {
    public:
        using element = std::pair<const std::thread::id, ShadowStack *>;
        using type = std::unordered_map<std::thread::id, ShadowStack*>;
        using iterator = type::iterator;
    public:
        static ShadowStack& stack_of_current_thread();

        void initialize_for_current_thread();
        void destroy_for_current_thread();
    public:

        void visit(const std::function<void(element &)>& fn) noexcept;
    private:
        static thread_local ShadowStack my_stack;

    private:
        SpinLock spinLock;
        type map;
    };
}