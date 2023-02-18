#pragma once

#include "gc/containers/ShadowStack.h"

namespace gccpp {

    class ThreadCtx;
    class GlobalCtx;
    class MarkWord;

    template<typename T>
    class Handle;

    template<typename T>
    class GcPtr;

    class HandleMark final {
        template<typename T>
        friend class Handle;
    public:
        explicit HandleMark(GlobalCtx& ctx);
        explicit HandleMark(ThreadCtx& threadCtx);

        ~HandleMark() {
            m_stack.leave(saved_sp);
        }

    private:
        std::size_t saved_sp; //todo simplify???
        details::ShadowStack &m_stack;
    };

    template<typename T>
    class Handle final {
    public:
        using pointer = T *;
        using reference = T *;

    public:
        explicit Handle(HandleMark &hm, const details::ObjectPointer &oop) :
                oop_ptr(hm.m_stack.push(oop)) {}

        explicit Handle(HandleMark &hm, const GcPtr<T> &oop) :
                oop_ptr(hm.m_stack.push(oop)) {}

    public:
        explicit operator bool() const {
            return (*oop_ptr).mw() != nullptr; //Todo read bar??
        }

        inline pointer operator->() const {
            return oop_ptr->content<T>();
        }

        inline reference operator*() const {
            return oop_ptr->content<T>();
        }

        [[nodiscard]]
        details::ObjectPointer oop() const {
            return *oop_ptr; // Todo need read bar??
        }

    public:
        inline Handle &operator=(std::nullptr_t) {
            *oop_ptr = nullptr;
            return *this;
        }

        Handle &operator=(void *ptr) = delete;

    private:
        details::ObjectPointer* oop_ptr;
    };

    static_assert(sizeof(Handle<int>) == sizeof(void *), "too large for small object");
}
