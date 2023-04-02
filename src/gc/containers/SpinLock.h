#pragma once
#include <atomic>

namespace gccpp::details {
    //See https://rigtorp.se/spinlock/
    class SpinLock final {
    public:
        SpinLock() = default;
        ~SpinLock() = default;

    public:
        inline void lock() {
            for (;;) {
                if (!locked.exchange(true, std::memory_order_acquire)) {
                    break;
                }
                while (locked.load(std::memory_order_relaxed)) {
                    __builtin_ia32_pause(); //todo
                }
            }
        }

        inline void unlock() {
            locked.store(false, std::memory_order_release);
        }

    private:
        alignas(64) std::atomic_bool locked{false}; // todo cache line size
    };

    static_assert(sizeof(SpinLock) == 64, "expect given size");
}
