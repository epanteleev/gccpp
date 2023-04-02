#pragma once
#include "gc/MarkWord.h"
#include "gc/allocators/Allocator.h"
#include "gc/containers/SpinLock.h"

#include <functional>

namespace gccpp {

    struct Chunk final {
    public:
        [[nodiscard]]
        inline std::size_t object_size() const {
            return chunk_size - sizeof(Chunk);
        }

        inline MarkWord* mw() {
            return reinterpret_cast<MarkWord*>(reinterpret_cast<std::byte*>(this) + sizeof(Chunk));
        }
    public:
        unsigned char magic;
        int chunk_size;
    };

    static_assert(sizeof(Chunk) == 8, "Expect given size of the structure.");

    class LinearAllocator final : public Allocator {
    public:
        static constexpr unsigned char MAGIC = 0xAA;
    public:
        explicit LinearAllocator(std::size_t _max_size_in_byte);
        ~LinearAllocator() override;

    public:
        void* alloc(std::size_t size) override;
        void free(void* addr) override;
        void print(std::ostringstream &out) override;

        void release() noexcept;
        bool contains(void* object_address) const noexcept;

    public:
        void visit(const std::function<void(Chunk*)>& fn) noexcept;

    public:
        static std::size_t align(std::size_t value) noexcept {
            return ((value + 7) / 8) * 8;
        }

        static Chunk* header(const void* object_address) noexcept;
    private:
        void* start_ptr{};
        const std::size_t max_size{};
        std::size_t offset{};
        std::size_t allocation_count{};
        details::SpinLock lock;
    };
}


