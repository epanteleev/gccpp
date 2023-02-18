#pragma once

#include <cstddef>
#include <utility>
#include "ObjectPointer.h"
#include "header/MarkWord.h"

#include "gc/Handle.h"
#include "gc/GCCollected.h"

namespace gccpp {
    class MarkWord;
    template<typename T>
    class Handle;

    template<typename T>
    class GcPtr: public details::ObjectPointer {
    public:
        using pointer = T *;

    public:
        GcPtr() = default;

        GcPtr(MarkWord *meta): ObjectPointer(meta) {}

        template<typename U>
        GcPtr(const GcPtr<U> &r): ObjectPointer(r.p) {}

        GcPtr(const GcPtr &r): ObjectPointer(r.p) {}

        GcPtr(Handle<T> &r);

        GcPtr(GcPtr &&r) noexcept:
                ObjectPointer(std::exchange(r.p, nullptr)) {}

    public:
        template<typename U>
        GcPtr<T> &operator=(const GcPtr<U> &r) { //todo check convertable
            p = r.p;
            return *this;
        }

        GcPtr<T> &operator=(const GcPtr &r) {
            if (this == &r) {
                return *this;
            }
            p = r.p;
            return *this;
        }

        GcPtr<T> &operator=(GcPtr &&r) noexcept {
            p = std::exchange(r.p, nullptr);
            return *this;
        }

        GcPtr<T> &operator=(Handle<T> &r) noexcept;

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

        inline bool operator==(const Handle<T> &r) const;
        inline bool operator!=(const Handle<T> &r) const;

    public:
        template<typename Type>
        static GcPtr<Type> from(gccpp::GCCollected* self) noexcept {
            return GcPtr<Type>(reinterpret_cast<MarkWord*>(reinterpret_cast<std::byte*>(self) - sizeof(MarkWord)));
        }
    };

    static_assert(sizeof(GcPtr<int>) == sizeof(void *), "too large for small object");
}