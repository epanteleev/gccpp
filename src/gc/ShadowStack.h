#pragma once
#include <array>
#include <stack>
#include "pointer/ObjectPointer.h"

namespace gccpp::details {
    class ShadowStack final {
    public:
        using size_type = std::size_t;
        using iterator = ObjectPointer*;
    private:
        static constexpr size_type MAX_STACK_SIZE = 2048;
    public:
        ShadowStack() = default;
        ~ShadowStack() = default;

    public:
        [[nodiscard]]
        size_type enter() const {
            return sp;
        }

        void leave(size_type saved_sp) noexcept {
            sp = saved_sp;
        }

        ObjectPointer* push(const ObjectPointer& ptr);

    public:
        [[nodiscard]]
        size_type size() const noexcept {
            return sp;
        }

        ObjectPointer operator[](size_type idx) {
            return stack[idx];
        }

    private:
        size_type sp{};
        std::array<ObjectPointer, MAX_STACK_SIZE> stack;
    };
}
