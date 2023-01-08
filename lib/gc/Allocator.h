#pragma once
#include <vector>

namespace tgc {
    class Allocator {
    public:
        virtual void* alloc(std::size_t size) = 0;
        virtual void free_dead(std::vector<void*>&& live_set) = 0;
    };

    class NaiveAllocator: public Allocator {
    public:
        NaiveAllocator() = default;
        ~NaiveAllocator();

    public:
        void* alloc(std::size_t size) override;
        void free_dead(std::vector<void*>&& live_set) override;
    private:
        std::vector<void*> allocated;
    };
}
