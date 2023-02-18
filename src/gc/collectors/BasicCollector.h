#pragma once
#include "gc/containers/ThreadsStacks.h"
#include "gc/allocators/Allocator.h"
#include "gc/Handle.h"
#include "pointer/GcPtr.h"

#include <cstring>
#include <cstdio>
#include <cassert>

namespace gccpp {

    class GlobalCtx;

    class BasicCollector {
        friend class GlobalCtx;
    public:
        explicit BasicCollector(Allocator* _allocator):
            allocator(_allocator){}

        virtual ~BasicCollector() = default;

    public:
        template<typename T, typename... Args>
        inline GcPtr<T> alloc(Args &&... args) {
            std::size_t size = sizeof(MarkWord) + sizeof(T);
            auto *ptr = static_cast<std::byte *>(allocator->alloc(size));
            if (ptr == nullptr) {
                assert(false);
                return GcPtr<T>(); //Todo impl logic of OOM.
            }

            std::memset(ptr, 0, sizeof(MarkWord));

            auto start = ptr + sizeof(MarkWord);
            new(start) T(std::forward<Args>(args)...);

            //auto* oop_ptr = push(details::ObjectPointer((MarkWord*)ptr));
            return GcPtr<T>(reinterpret_cast<MarkWord *>(ptr));
        }
    public:
        Allocator* get_allocator() noexcept {
            return allocator;
        }

        [[nodiscard]]
        GlobalCtx* context() const noexcept {
            assert(ctx != nullptr);
            return ctx;
        }
    private:
        void set_ctx(GlobalCtx* _ctx);
        void start_collection();
        details::ObjectPointer* push(const details::ObjectPointer& ptr); //Todo temporal

    protected:
        virtual void collect() = 0;

    protected:
        GlobalCtx* ctx{};
        Allocator* allocator;
    };
}