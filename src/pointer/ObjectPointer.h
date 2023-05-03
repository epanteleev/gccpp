#pragma once
#include "gc/MarkWord.h"
#include "gc/GarbageCollected.h"
#include "macros.h"

namespace gccpp::details {

    class ObjectPointer {
    public:
        ObjectPointer() = default;
        ~ObjectPointer() = default;

        explicit ObjectPointer(MarkWord* ptr) : p(ptr) {}

        ObjectPointer(const ObjectPointer& ptr) : p(ptr.mw()) {}
    public:
        [[nodiscard]]
        always_inline MarkWord* mw() const noexcept {
            return p;
        }

        always_inline void update(void* address) noexcept {
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
        ObjectPointer &operator=(const ObjectPointer& ptr) noexcept = default;

    public:
        template<GarbageCollectedType Type>
        [[nodiscard]]
        inline Type* content() const noexcept {
            return static_cast<Type*>(p->object());
        }

        void trace(GCOperation* op) const noexcept {
            content<GarbageCollected>()->trace(op);
        }

    protected:
        MarkWord* p{};
    };
}