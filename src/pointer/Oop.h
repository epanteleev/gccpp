#pragma once

#include <cstddef>
#include <utility>
#include "ObjectPointer.h"
#include "gc/MarkWord.h"

#include "Handle.h"
#include "gc/GarbageCollected.h"
#include "gc/fwd.h"

namespace gccpp {

    template<typename T>
    class Oop: public details::ObjectPointer {
    public:
        using pointer = T *;
    public:
        Oop() = default;

        Oop(MarkWord *meta): ObjectPointer(meta) {}

        template<typename U>
        Oop(const Oop<U> &r): ObjectPointer(r.p) {}

        Oop(const Oop &r): ObjectPointer(r.p) {}

        Oop(Handle<T> &r);

        Oop(Oop &&r) noexcept:
                ObjectPointer(std::exchange(r.p, nullptr)) {}

    public:
        Oop<T> &operator=(const Oop<T> &r);
        Oop<T> &operator=(Oop &&r) noexcept;
        Oop<T> &operator=(Handle<T> &r) noexcept;

        inline pointer operator->() const;
        inline T &operator*() const;

        explicit operator bool() const;

        inline bool operator==(const Oop &r) const;
        inline bool operator==(const Handle<T> &r) const;
        inline bool operator!=(const Handle<T> &r) const;
        inline bool operator!=(const Oop &r) const;
    public:
        static Oop<T> from(gccpp::GarbageCollected* self) noexcept {
            return Oop<T>(reinterpret_cast<MarkWord*>(reinterpret_cast<std::byte*>(self) - sizeof(MarkWord)));
        }

        static constexpr std::size_t sizeOf() {
            return sizeof(gccpp::Oop<gccpp::GarbageCollected>);
        }
    };

    static_assert(sizeof(Oop<GarbageCollected>) == sizeof(void *), "too large for small object");
}