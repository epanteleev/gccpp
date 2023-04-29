#pragma once
#include <cstddef>
#include <cassert>
#include <cstring>
#include "macros.h"


namespace gccpp {
    //Todo special mw for particular BasicCollector algo.
    class MarkWord final {
        static constexpr std::size_t FWD_MASK = 0xFFFF0000'00000000;
    public:
        enum class Color : unsigned char {
            White,
            Black
        };

    public:
        MarkWord() = default;
        ~MarkWord() = default;

        [[nodiscard]]
        always_inline void *object() const {
            return (void *) ((char*)this + sizeof(MarkWord));
        }

    public:
        always_inline void set_color(Color new_color) {
            m_color = new_color;
        }

        always_inline void set_forwarding_ptr(void* new_fp) {
            assert((reinterpret_cast<std::size_t>(new_fp) & FWD_MASK) == 0);
            std::memcpy(&forwarding_pointer, &new_fp, 7);
        }

        [[nodiscard]]
        always_inline Color color() const {
            return static_cast<Color>(m_color);
        }

        [[nodiscard]]
        always_inline void* forwarding_ptr() const {
            // Todo for ARM64 (???): return *reinterpret_cast<void**>(const_cast<MarkWord*>(this));
            void* ret{};
            std::memcpy(&ret, &forwarding_pointer, 7);
            return ret;
        }

    private:
        Color m_color{};
        char forwarding_pointer[7]{};
    };

    static_assert(sizeof(MarkWord) == sizeof(void *), "too large for small allocation");
}