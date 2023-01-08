#pragma once

#include <cstddef>
#include <utility>
#include "ObjectPointer.h"
#include "header/MarkWord.h"
#include "GcRoot.h"
#include "gc/GCCollected.h"

namespace gccpp {
    class MarkWord;

    template<typename T>
    class GcPtr: public details::ObjectPointer {
    public:
        using pointer = T *;

    public:
        GcPtr() = default;

        GcPtr(MarkWord *meta): ObjectPointer(meta) {}

        template<typename U>
        GcPtr(const GcPtr<U> &r): ObjectPointer(r.p) {}

        template<typename U>
        GcPtr(const GcRoot<U> &r): ObjectPointer(r()) {}

        GcPtr(const GcPtr &r): ObjectPointer(r.p) {}

        GcPtr(const GcRoot<T> &r): ObjectPointer(r()) {}

        GcPtr(GcPtr &&r) noexcept:
                ObjectPointer(std::exchange(r.p, nullptr)) {}

    public:
        template<typename U>
        GcPtr &operator=(const GcPtr<U> &r) {
            p = r.p;
            return *this;
        }

        GcPtr &operator=(const GcPtr &r) {
            if (this == &r) {
                return *this;
            }
            p = r.p;
            return *this;
        }

        GcPtr &operator=(GcPtr &&r) noexcept {
            p = std::exchange(r.p, nullptr);
            return *this;
        }

        inline pointer operator->() const {
            return static_cast<pointer>(p->object());
        }

        inline T &operator*() const {
            return *static_cast<pointer>(p->object());
        }

        explicit operator bool() const {
            return p != nullptr;
        }

        inline bool operator==(const GcPtr &r) const {
            return p == r.p;
        }

        inline bool operator!=(const GcPtr &r) const {
            return p != r.p;
        }

        inline bool operator==(const GcRoot<T> &r) const {
            return p == r();
        }

        inline bool operator!=(const GcRoot<T> &r) const {
            return p != r();
        }

    public:
        template<typename Type>
        static GcPtr<Type> from(gccpp::GCCollected* self) noexcept {
            return GcPtr<Type>(reinterpret_cast<MarkWord*>(reinterpret_cast<std::byte*>(self) - sizeof(MarkWord)));
        }
    };

    static_assert(sizeof(GcPtr<int>) == sizeof(void *), "too large for small object");
}