#pragma once
#include "pointer/GcRoot.h"
#include "gc/ShadowStack.h"
#include "gc/allocators/Allocator.h"
#include "gc/Frame.h"

#include <cstring>
#include <cstdio>

namespace gccpp {

    class GC {
    public:
        explicit GC(Allocator* _allocator): allocator(_allocator) {}
        virtual ~GC() = default;

    public:
        template<typename T, typename... Args>
        inline GcRoot<T> gc_alloc(Args &&... args) {
            std::size_t size = sizeof(MarkWord) + sizeof(T);
            auto *ptr = static_cast<std::byte *>(allocator->alloc(size));
            if (ptr == nullptr) {
                assert(false);
                return GcRoot<T>(); //Todo impl logic of OOM.
            }

            std::memset(ptr, 0, sizeof(MarkWord));
            auto start = ptr + sizeof(MarkWord);
            new(start) T(std::forward<Args>(args)...);

            auto* oop_ptr = shadowStack.push(details::ObjectPointer((MarkWord*)ptr));
            return GcRoot<T>(oop_ptr);
        }
    public:
        [[nodiscard]]
        inline Frame enter() {
            return Frame(shadowStack);
        }

        details::ShadowStack& stack() noexcept {
            return shadowStack;
        }

        Allocator* get_allocator() noexcept {
            return allocator;
        }
    public:
        virtual void safepoint_at_poll() = 0;

    protected:
        details::ShadowStack shadowStack;
        Allocator* allocator;
    };
}