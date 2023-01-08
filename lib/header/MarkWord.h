#pragma once

#include <atomic>

namespace tgc {
    class MarkWord final {
    public:
        enum class Color : unsigned char {
            White,
            Black
        };

    public:
        MarkWord(std::size_t _sizeOf):
            color(Color::White),
            sizeOf(_sizeOf) {}

        ~MarkWord() = default;

        [[nodiscard]]
        void *objPtr() const;

    public:
        Color color;
        std::size_t sizeOf;
    };

    static_assert(sizeof(MarkWord) <= sizeof(void *) * 2, "too large for small allocation");
}