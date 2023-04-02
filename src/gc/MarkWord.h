#pragma once
#include <cstddef>

namespace gccpp {
    //Todo special mw for particular BasicCollector algo.
    class MarkWord final {
    public:
        enum class Color : unsigned char {
            White,
            Black
        };

    public:
        MarkWord(): color(Color::White) {}
        ~MarkWord() = default;

        [[nodiscard]]
        void *object() const {
            return (void *) ((char*)this + sizeof(MarkWord));
        }

    public:
        Color color{};
        void * forwarding_pointer{};
    };

    static_assert(sizeof(MarkWord) <= sizeof(void *) * 2, "too large for small allocation");
}