#pragma once
#include <array>
#include <stack>
#include "pointer/ObjectPointer.h"
#include "Buffer.h"

namespace gccpp::details {
    class ShadowStack final {
    public:
        using size_type = std::size_t;

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
            stack.trunc(saved_sp);
        }

        ObjectPointer* push(const ObjectPointer& ptr);

    public:
        [[nodiscard]]
        size_type size() const noexcept {
            return sp;
        }

        [[nodiscard]]
        inline ObjectPointer at(size_type idx) const noexcept {
            return stack[idx];
        }

        inline ObjectPointer* addr(size_type idx) noexcept {
            return &stack[idx];
        }
    private:
        size_type sp{};
        Buffer<ObjectPointer> stack;
    };
}
