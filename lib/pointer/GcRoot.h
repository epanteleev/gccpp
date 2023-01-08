#pragma once
#include <utility>
#include "ObjectPointer.h"

namespace tgc {
    class MarkWord;

    template<typename T>
    class GcRoot final {
    public:
        using pointer = T *;
        using element_type = T;

    public:
        GcRoot() = default;

        explicit GcRoot(details::ObjectPointer* _oop_ptr) : oop_ptr(_oop_ptr) {}

        template<typename U>
        explicit GcRoot(const GcRoot<U> &r): oop_ptr(r.oop_ptr) {}

        GcRoot(const GcRoot &r) : oop_ptr(r.oop_ptr) {}

    public:
//        template<typename U>
//        GcRoot &operator=(const GcRoot<U> &r) {
//            oop_ptr = r.p;
//            return *this;
//        }
//
//        GcRoot &operator=(const GcRoot &r) {
//            if (this == &r) {
//                return *this;
//            }
//            p = r.p;
//            return *this;
//        }

    public:
        pointer operator->() const {
            return oop_ptr->content<T>(); //Todo read bar??
        }

        T &operator*() const {
            return *oop_ptr->content<T>(); //Todo read bar??
        }

        explicit operator bool() const {
            return (*oop_ptr).markWord() != nullptr; //Todo read bar??
        }

        [[nodiscard]]
        details::ObjectPointer operator()() const {
            return *oop_ptr; // Todo need read bar??
        }
    public:

        inline GcRoot &operator=(std::nullptr_t) {
            *oop_ptr = nullptr;
            return *this;
        }

        GcRoot &operator=(T *ptr) = delete;

    private:
        details::ObjectPointer* oop_ptr;
    };

    static_assert(sizeof(GcRoot<int>) <= sizeof(void *) * 3, "too large for small object");
}