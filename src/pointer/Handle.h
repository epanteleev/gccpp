#pragma once

#include "gc/fwd.h"
#include <cstdint>
#include "pointer/ObjectPointer.h"

namespace gccpp {
    class HandleMark final {
    public:
        HandleMark();
        ~HandleMark();

    private:
        std::size_t saved_sp{};
    };

    template<typename T>
    class Handle final {
    public:
        using pointer = T *;
        using reference = T *;

    public:
        Handle(const details::ObjectPointer &oop);
        Handle(const Oop<T> &oop);

    public:
        explicit operator bool() const;

        inline pointer operator->() const;

        inline reference operator*() const;

        [[nodiscard]]
        details::ObjectPointer oop() const;

    public:
        Handle<T> &operator=(Oop<T> ptr);
        Handle<T> &operator=(const Handle<T>& ptr);
    private:
        details::ObjectPointer* oop_ptr;
    };

    static_assert(sizeof(Handle<GarbageCollected>) == sizeof(void *), "too large for small object");
}
