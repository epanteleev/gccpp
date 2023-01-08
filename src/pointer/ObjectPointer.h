#pragma once
#include "header/MarkWord.h"

namespace gccpp::details {
    class ObjectPointer {
    public:
        ObjectPointer() = default;
        ~ObjectPointer() = default;

        explicit ObjectPointer(MarkWord* ptr) : p(ptr) {}

        ObjectPointer(const ObjectPointer& ptr) : p(ptr.mw()) {}
    public:
        MarkWord* mw() const noexcept {
            return p; //Todo
        }

        void update(void* address) noexcept {
            p = static_cast<MarkWord*>(address);
        }
    public:
        inline bool operator==(std::nullptr_t) const {
            return p == nullptr;
        }

       ObjectPointer& operator=(std::nullptr_t) {
            p = nullptr;
            return *this;
        }

        inline bool operator!=(std::nullptr_t) const {
            return p != nullptr;
        }

        ObjectPointer &operator=(void *ptr) = delete;
    public:
        template<typename Type>
        Type* content() const noexcept {
            return static_cast<Type*>(p->object());
        }

    public:
        MarkWord* p{};
    };
}