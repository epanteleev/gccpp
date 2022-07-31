#pragma once
#include <utility>
#include "PtrBase.h"

namespace tgc {
    class ObjMeta;

    template<typename T>
    class GcRoot : public details::PtrBase {
    public:
        using pointer = T *;
        using element_type = T;

    public:
        GcRoot() = default;

        explicit GcRoot(ObjMeta *meta) : PtrBase(meta) {}

        template<typename U>
        explicit GcRoot(const GcRoot<U> &r): PtrBase(r.p) {}

        GcRoot(const GcRoot &r) : PtrBase(r.p) {}

    public:
        template<typename U>
        GcRoot &operator=(const GcRoot<U> &r) {
            p = r.p;
            return *this;
        }

        GcRoot &operator=(const GcRoot &r) {
            if (this == &r) {
                return *this;
            }
            p = r.p;
            return *this;
        }

    public:
        pointer operator->() const {
            return static_cast<pointer>(p->objPtr());
        }

        T &operator*() const {
            return *static_cast<pointer>(p->objPtr());
        }

        explicit operator bool() const { return p != nullptr; }

    public:
        inline bool operator==(const GcRoot &r) const {
            return p == r.p;
        }

        inline bool operator!=(const GcRoot &r) const {
            return p != r.p;
        }

        inline GcRoot &operator=(std::nullptr_t) {
            p = nullptr;
            return *this;
        }

        GcRoot &operator=(T *ptr) = delete;
    };

    static_assert(sizeof(GcRoot<int>) <= sizeof(void *) * 3, "too large for small object");
}