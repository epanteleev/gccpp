#pragma once

#include <cstddef>
#include <utility>
#include "PtrBase.h"
#include "meta/ObjMeta.h"
#include "GcRoot.h"

namespace tgc {
    class ObjMeta;

    template<typename T>
    class GcPtr: public details::PtrBase {
    public:
        using pointer = T *;
        using element_type = T;

    public:
        GcPtr() = default;

        explicit GcPtr(ObjMeta *meta): PtrBase(meta) {}

        template<typename U>
        explicit GcPtr(const GcPtr<U> &r): PtrBase(r.p) {}

        template<typename U>
        explicit GcPtr(const GcRoot<U> &r): PtrBase(r.p) {}

        GcPtr(const GcPtr &r): PtrBase(r.p) {}

        GcPtr(const GcRoot<T> &r): PtrBase(r.p) {}

        GcPtr(GcPtr &&r) noexcept:
            PtrBase(std::exchange(r.p, nullptr)) {}

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
            return static_cast<pointer>(p->objPtr());
        }

        inline T &operator*() const {
            return *static_cast<pointer>(p->objPtr());
        }

        explicit operator bool() const { return p != nullptr; }

        inline bool operator==(const GcPtr &r) const {
            return p == r.p;
        }

        inline bool operator!=(const GcPtr &r) const {
            return p != r.p;
        }

        inline bool operator==(const GcRoot<T> &r) const {
            return p == r.p;
        }

        inline bool operator!=(const GcRoot<T> &r) const {
            return p != r.p;
        }

        GcPtr &operator=(T *ptr) = delete;

        GcPtr &operator=(std::nullptr_t) {
            p = nullptr;
            return *this;
        }
    };

    static_assert(sizeof(GcPtr<int>) <= sizeof(void *) * 3, "too large for small object");
}