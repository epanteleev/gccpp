#pragma once

namespace gccpp::details::mem {
    inline static void write_barrier() {
        asm volatile("": : :"memory");
    }

    constexpr std::size_t align(std::size_t value) noexcept {
        return ((value + 7) / 8) * 8;
    }

    template<typename Type>
    constexpr std::size_t aligned_sizeof() noexcept {
        return align(sizeof(Type));
    }
}
