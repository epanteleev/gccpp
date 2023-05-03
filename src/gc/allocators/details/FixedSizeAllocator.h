#pragma once

#include "gc/MarkWord.h"
#include "gc/containers/SpinLock.h"
#include "gc/containers/Memory.h"
#include "gc/containers/Page.h"
#include <sstream>
#include <functional>
#include <memory>

namespace gccpp::details {

    template<std::size_t SIZE>
    class RightFinger;

    template<std::size_t SIZE>
    class LeftFinger;

    template<std::size_t SIZE>
    class Finger {
    public:
        explicit Finger(std::size_t addr, std::size_t _end):
                address(addr),
                end(_end) {}

        virtual ~Finger() = default;
    public:
        void* operator()() noexcept {
            return std::assume_aligned<SIZE>(reinterpret_cast<void*>(address)); //Todo investigate impact
        }

        [[nodiscard]]
        std::size_t addr() const noexcept {
            return address;
        }

    public:
        static bool crossed(LeftFinger<SIZE>& left, RightFinger<SIZE>& right) noexcept;

    protected:
        std::size_t address;
        std::size_t end;
    };

    template<std::size_t SIZE>
    class RightFinger: public Finger<SIZE> {
        static_assert(mem::align(SIZE) == SIZE, "expect aligned value");
    public:
        explicit RightFinger(std::size_t addr, std::size_t end):
            Finger<SIZE>(addr, end) {}

    public:
        RightFinger next() noexcept {
            assert(has());
            auto object = reinterpret_cast<MarkWord*>(this->address);
            while (object->color() != MarkWord::Color::Black) {
                this->address -= SIZE;
                if (!has()) {
                    return RightFinger(this->end, this->end);
                }
                object = reinterpret_cast<MarkWord*>(this->address);
            }
            return RightFinger(this->address, this->end);
        };

        bool has() noexcept {
            return this->address >= this->end;
        }
    };

    template<std::size_t SIZE>
    class LeftFinger: public Finger<SIZE> {
        static_assert(mem::align(SIZE) == SIZE, "expect aligned value");
    public:
        explicit LeftFinger(std::size_t addr, std::size_t _end):
            Finger<SIZE>(addr, _end) {}

    public:
        LeftFinger next() noexcept {
            assert(has());
            auto object = reinterpret_cast<MarkWord*>(this->address);
            while (object->color() != MarkWord::Color::White) {
                this->address += SIZE;
                if (!has()) {
                    return LeftFinger(this->end, this->end);
                }
                object = reinterpret_cast<MarkWord*>(this->address);
            }
            return LeftFinger(this->address, this->end);
        }

        bool has() noexcept {
            return this->address < this->end;
        }
    };

    template<std::size_t SIZE>
    bool Finger<SIZE>::crossed(LeftFinger<SIZE> &left, RightFinger<SIZE> &right) noexcept {
        return left() >= right();
    }

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
                << "Object size: " << SIZE << "\n"
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

        void refresh_offset(RightFinger<SIZE>& right) noexcept {
            auto new_offset = right.addr() - reinterpret_cast<std::size_t>(start_ptr) - SIZE;
            assert(new_offset <= offset);
            offset = new_offset;
        }
    public:
        LeftFinger<SIZE> left_finger() noexcept {
            auto end = reinterpret_cast<std::size_t>(start_ptr) + offset;
            LeftFinger<SIZE> left(reinterpret_cast<std::size_t>(start_ptr), end);
            left.next();
            return left;
        }

        RightFinger<SIZE> right_finger() noexcept {
            assert(offset >= SIZE);
            auto last = reinterpret_cast<std::size_t>(start_ptr) + offset - SIZE;
            RightFinger<SIZE> right(last, reinterpret_cast<std::size_t>(start_ptr));
            right.next();

            return right;
        }

    public:
        void* start_ptr{};
        const std::size_t max_size{};
        std::size_t offset{};
        details::SpinLock lock;
    };

}