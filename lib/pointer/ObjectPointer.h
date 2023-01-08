#pragma once
#include "header/MarkWord.h"

namespace tgc::details {
    class ObjectPointer {
    public:
        ObjectPointer() = default;
        ~ObjectPointer() = default;

        ObjectPointer(MarkWord* ptr) : p(ptr) {}

        ObjectPointer(const ObjectPointer& ptr) : p(ptr.markWord()) {}
    public:
        MarkWord* markWord() const noexcept {
            return p; //Todo
        }

    public:
        MarkWord::Color color() const {
            return p->color;
        }
    public:
        inline bool operator==(std::nullptr_t) const {
            return p == nullptr;
        }

        inline bool operator!=(std::nullptr_t) const {
            return p != nullptr;
        }

    public:
        template<typename Type>
        Type* content() const noexcept {
            return static_cast<Type*>(p->objPtr());
        }

    public:
        MarkWord* p{};
    };
}