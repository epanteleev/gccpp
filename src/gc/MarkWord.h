#pragma once
#include <cstddef>
#include "macros.h"


namespace gccpp {
    //Todo special mw for particular BasicCollector algo.
    class MarkWord final {
        static constexpr std::size_t COLOR_MASK = 0xFF000000'00000000;
        static constexpr std::size_t FRDPTR_MASK = ~COLOR_MASK;

    public:
        enum class Color : unsigned char {
            White,
            Black
        };

    public:
        MarkWord() {}
        ~MarkWord() = default;

        [[nodiscard]]
        always_inline void *object() const {
            return (void *) ((char*)this + sizeof(MarkWord));
        }

    public:
        always_inline void set_color(Color new_color) {
            m_color = static_cast<unsigned char>(new_color);
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
        unsigned char m_color{};
        unsigned long forwarding_pointer:56{};
    };

    static_assert(sizeof(MarkWord) == sizeof(void *), "too large for small allocation");
}