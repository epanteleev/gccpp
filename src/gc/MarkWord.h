#pragma once
#include <cstddef>
#include <cassert>
#include "macros.h"


namespace gccpp {
    //Todo special mw for particular BasicCollector algo.
    class MarkWord final {
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
            forwarding_pointer = reinterpret_cast<unsigned long>(new_fp);
        }

        [[nodiscard]]
        always_inline Color color() const {
            return static_cast<Color>(m_color);
        }

        [[nodiscard]]
        always_inline void* forwarding_ptr() const {
            return reinterpret_cast<void*>(forwarding_pointer);
        }

    private:
        Color m_color{};
        unsigned long forwarding_pointer{};
    };

    static_assert(sizeof(MarkWord) == 2 * sizeof(void *), "too large for small allocation");
}