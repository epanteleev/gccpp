#pragma once
#include "gc/MarkWord.h"
#include "gc/containers/SpinLock.h"

#include <functional>

namespace gccpp {

    struct Chunk final {
    public:
        [[nodiscard]]
        inline std::size_t object_size() const {
            auto actual_size = chunk_size - sizeof(Chunk);
            assert(actual_size > 0);
            return actual_size;
        }

        inline MarkWord* mw() {
            return reinterpret_cast<MarkWord*>(reinterpret_cast<std::byte*>(this) + sizeof(Chunk));
        }
    public:
        unsigned char magic;
        int chunk_size;
    };

    static_assert(sizeof(Chunk) == 8, "Expect given size of the structure.");

    class LinearAllocator final {
    public:
        static constexpr unsigned char MAGIC = 0xAA;
    public:
        explicit LinearAllocator(std::size_t _max_size_in_byte);
        ~LinearAllocator();

    public:
        void* alloc(std::size_t size) noexcept;
        void print(std::ostringstream &out);

        void release() noexcept;
        bool contains(void* object_address) const noexcept;

    public:
        void visit(const std::function<void(Chunk*)>& fn) noexcept;

    public:

        static Chunk* header(const void* object_address) noexcept;
    private:
        void* start_ptr{};
        const std::size_t max_size{};
        std::size_t offset{};
        std::size_t allocation_count{};
        details::SpinLock lock;
    };
}


