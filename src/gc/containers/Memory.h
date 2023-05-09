#pragma once

namespace gccpp::details::mem {
    inline static void write_barrier() {
        asm volatile("": : :"memory");
       // __sync_synchronize();
    }

    inline static bool xchgb(const bool volatile* dest, bool value) {
        __asm__ volatile ("xchgb (%2),%0"
                : "=q" (value)
                : "0" (value), "r" (dest)
                : "memory");
        return value;
    }

    constexpr std::size_t align8(std::size_t value) noexcept {
        return ((value + 7) / 8) * 8;
    }

    template<typename Type>
    constexpr std::size_t aligned_sizeof8() noexcept {
        return align8(sizeof(Type));
    }
}