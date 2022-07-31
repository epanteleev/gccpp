#pragma once

#include <atomic>

namespace tgc {

    class ClassMeta;

    class ObjMeta final {
    public:
        enum class Color : unsigned char {
            White,
            Gray,
            Black
        };
        ClassMeta *klass = nullptr;
        Color color = Color::White;

    public:
        ObjMeta() :
                klass(nullptr),
                color(Color::White) {}

        ~ObjMeta() = default;

        [[nodiscard]]
        void *objPtr() const;
    };

    static_assert(sizeof(ObjMeta) <= sizeof(void *) * 2, "too large for small allocation");
}