#pragma once
#include <cstddef>

namespace gccpp {
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
            return (void *) ((std::byte*)this + sizeof(MarkWord));
        }

    public:
        Color color{};
        unsigned int size{}; // as uint
    };

    static_assert(sizeof(MarkWord) <= sizeof(void *), "too large for small allocation");
}