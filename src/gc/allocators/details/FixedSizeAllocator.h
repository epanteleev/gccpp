#pragma once

#include "gc/MarkWord.h"
#include "gc/containers/SpinLock.h"
#include "gc/containers/Memory.h"
#include "gc/containers/Page.h"
#include <sstream>
#include <functional>

namespace gccpp::details {

    template<std::size_t SIZE>
    class FixedSizeAllocator final {
        static_assert(mem::align(SIZE) == SIZE, "expect aligned value");
    public:
        explicit FixedSizeAllocator(std::size_t _max_size):
            max_size(mem::align(_max_size))
        {
            assert(max_size >= SIZE);
            start_ptr = Page::alloc(max_size);
            assert(reinterpret_cast<std::size_t>(start_ptr) % 8 == 0);
        }

        ~FixedSizeAllocator() {
            Page::free(start_ptr, max_size);
        }
    public:
        void* alloc() noexcept {
            const std::lock_guard<details::SpinLock> _l(lock);

            const std::size_t current_address = reinterpret_cast<std::size_t>(start_ptr) + offset;
            if (SIZE + offset > max_size) {
                return nullptr;
            }
            offset += SIZE;
            return reinterpret_cast<void*>(current_address);
        }

        void print(std::ostringstream &out) {
            out << "Start: " << start_ptr << "\n"
                << "Max size: " << max_size << "\n"
                << "Offset: " << offset << "\n"
                << "Allocation count: " << allocation_count() << "\n";
        }

        [[nodiscard]]
        std::size_t allocation_count() const noexcept {
            return offset / SIZE;
        }

        void visit(const std::function<void(void*)>& fn) noexcept {
            if (offset == 0) {
                return;
            }
            auto current = static_cast<std::byte*>(start_ptr);
            do {
                fn(current);
                current = current + SIZE;
            } while(current < static_cast<std::byte*>(start_ptr) + offset);
        }

        bool contains(void* object_address) const noexcept {
            const auto end = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(start_ptr) + offset);
            return start_ptr <= object_address && object_address < end;
        }
    public:
        void* start_ptr{};
        const std::size_t max_size{};
        std::size_t offset{};
        details::SpinLock lock;
    };

}