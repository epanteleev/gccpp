#pragma once
#include "gc/containers/ThreadsStacks.h"
#include "gc/allocators/Allocator.h"
#include "pointer/Handle.h"
#include "pointer/Oop.h"

#include <cstring>
#include <cstdio>
#include <cassert>
#include "gc/fwd.h"

namespace gccpp {

    class BasicCollector {
        friend class Environment;
        friend class details::Reallocate;
        friend class details::Relocate;
    public:
        explicit BasicCollector(Allocator* _allocator):
            allocator(_allocator){}

        virtual ~BasicCollector() = default;

    public:
        [[nodiscard]]
        Environment* context() const noexcept {
            assert(ctx != nullptr);
            return ctx;
        }

    public:
        virtual void write_barrier(details::ObjectPointer* des, const details::ObjectPointer& src) {
            *des = src;
        }

    private:
        void set_ctx(Environment* _ctx);

    public:
        virtual void collect() = 0;

    protected:
        Environment* ctx{};
        Allocator* allocator;
    };
}