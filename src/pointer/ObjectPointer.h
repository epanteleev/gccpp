#pragma once
#include "header/MarkWord.h"
#include "gc/GCCollected.h"
#include <concepts>

namespace gccpp::details {
    template<class T>
    concept GCCollectedType = std::derived_from<T, GCCollected>;

    class ObjectPointer {
    public:
        ObjectPointer() = default;
        ~ObjectPointer() = default;

        explicit ObjectPointer(MarkWord* ptr) : p(ptr) {}

        ObjectPointer(const ObjectPointer& ptr) : p(ptr.mw()) {}
    public:
        [[nodiscard]]
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

        template<GCCollectedType Type>
        [[nodiscard]]
        Type* content() const noexcept {
            return static_cast<Type*>(p->object());
        }

        void trace(GCOperation* op) const noexcept {
            content<GCCollected>()->trace(op);
        }
    public:
        MarkWord* p{};
    };
}