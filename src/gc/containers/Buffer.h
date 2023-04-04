#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>

#include <vector>
#include "pointer/ObjectPointer.h"
#include "gc/containers/Page.h"

namespace gccpp {
    template<typename T>
    class Buffer final {
        static constexpr std::size_t INITIAL_SIZE = 512;
        static constexpr std::size_t RATIO = 2;

    public:
        Buffer() {
            buffer = static_cast<T*>(Page::alloc(INITIAL_SIZE * sizeof(T)));
            capacity = INITIAL_SIZE;
            current_size = 0;
        }

        ~Buffer() {
            Page::free(buffer, capacity * sizeof(T));
        }

    public:
        inline void push(const T oop) noexcept {
            if (current_size == capacity) [[unlikely]] {
                resize();
            }
            buffer[current_size] = oop;
            current_size += 1;
        }

        [[nodiscard]]
        inline bool empty() const noexcept {
            return current_size == 0;
        }

        inline T pop() noexcept {
            assert(current_size != 0);
            auto back = buffer[current_size - 1];
            current_size -= 1;
            return back;
        }
    private:
        void resize() {
            std::size_t new_capacity = capacity * RATIO;
            void* old_addr = static_cast<void*>(buffer);
            void* new_addr = Page::realloc(old_addr, capacity * sizeof(T), new_capacity * sizeof(T));
            buffer = static_cast<T*>(new_addr);
            capacity = new_capacity;
        }

    private:
        T* buffer;
        std::size_t capacity;
        std::size_t current_size;
    };
}

