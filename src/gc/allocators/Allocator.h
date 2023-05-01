#pragma once
#include <cstddef>
#include <iosfwd>

namespace gccpp {
    class Allocator {
    public:
        virtual ~Allocator() = default;
        virtual void* alloc(std::size_t size) noexcept = 0;
        virtual void print(std::ostringstream& out) = 0;
    };
}
