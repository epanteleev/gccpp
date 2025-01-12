#pragma once

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
        size_type enter() const noexcept {
            return size();
        }

        void leave(size_type saved_sp) noexcept {
            stack.trunc(saved_sp);
        }

        ObjectPointer* push(const ObjectPointer& ptr);

    public:
        [[nodiscard]]
        size_type size() const noexcept {
            return stack.size();
        }

        [[nodiscard]]
        inline ObjectPointer at(size_type idx) const noexcept {
            return stack[idx];
        }

        inline ObjectPointer* addr(size_type idx) noexcept {
            return &stack[idx];
        }
    private:
        Buffer<ObjectPointer> stack;
    };
}
