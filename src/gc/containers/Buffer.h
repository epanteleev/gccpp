#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "pointer/ObjectPointer.h"

namespace gccpp {
    template<typename T>
    class Buffer final {
        static constexpr std::size_t INITIAL_SIZE = 4096;
    public:
        Buffer() {
            buffer.reserve(INITIAL_SIZE);
        }
        ~Buffer() = default;

    public:
        inline void push(const T oop) noexcept {
            buffer.push_back(oop);
        }

        [[nodiscard]]
        inline bool empty() const noexcept {
            return buffer.empty();
        }

        inline T pop() noexcept {
            auto back = buffer.back();
            buffer.pop_back();
            return back;
        }
    public:
        std::vector<T> buffer;
    };
}

