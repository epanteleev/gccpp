#pragma once

#include "pointer/GcPtr.h"
#include "pointer/GcRoot.h"
#include "GcCollected.h"
#include "ShadowStack.h"
#include "Allocator.h"
#include "Frame.h"

#include <cstring>
#include <cstdio>

namespace tgc {

    class Gc final {
        friend class Mark;
        friend class Sweep;
    public:
        Gc() = default;
        ~Gc() = default;

    public:
        template<typename T, typename... Args>
        inline GcRoot<T> gc_alloc(Args &&... args) {
            std::size_t size = sizeof(MarkWord) + sizeof(T);
            auto *ptr = static_cast<std::byte *>(allocator.alloc(size));
            if (ptr == nullptr) {
                return GcRoot<T>();
            }

            std::memset(ptr, 0, size);
            auto start = ptr + sizeof(MarkWord);
            new(start) T(std::forward<Args>(args)...);

            auto* oop_ptr = shadowStack.push(details::ObjectPointer((MarkWord*)ptr));
            return GcRoot<T>(oop_ptr);
        }

        [[nodiscard]]
        inline Frame enter() {
            return Frame(shadowStack);
        }

        void safepoint();

    private:
        details::ShadowStack shadowStack;
        NaiveAllocator allocator;
    };
}