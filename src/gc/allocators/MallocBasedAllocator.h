#pragma once

#include "gc/allocators/Allocator.h"
#include "gc/containers/SpinLock.h"

#include <mutex>
#include <unordered_set>
#include <functional>
#include <cassert>

namespace gccpp {
    namespace malloc_based {
        struct AllocationStat {
            std::size_t allocation_count{};
            std::size_t allocated_size{};
        };

        struct Chunk {
        public:
            inline void* object() {
                return reinterpret_cast<char*>(this) + sizeof(Chunk);
            }
        public:
            static Chunk* make(void* addr) {
                return new(addr) Chunk();
            }

            static void* to_addr(Chunk* addr) {
                return reinterpret_cast<char*>(addr) - sizeof(Chunk);
            }
        public:
            Chunk* next{};
            Chunk* prev{};
        };

        struct AllocInfo {
            Chunk* first{};
            Chunk* last{};

        public:
            void append(Chunk* chunk) {
                assert(chunk != nullptr);
                if (first == nullptr) [[unlikely]] {
                    first = chunk;
                    last = chunk;
                    return;
                }
                last->next = chunk;
                chunk->prev = last;
                chunk->next = nullptr;

                last = last->next;
            }

            Chunk* remove(Chunk* chunk) {
                auto ret = chunk->next;
                if (chunk->prev != nullptr) {
                    chunk->prev->next = chunk->next;
                } else {
                    first = chunk->next;
                }

                if (chunk->next != nullptr) {
                    chunk->next->prev = chunk->prev;
                } else {
                    last = chunk->prev;
                }
                return ret;
            }
        };
    }

    class MallocBasedAllocator: public Allocator {
    public:
        explicit MallocBasedAllocator(std::size_t _max_size):
            max_size(_max_size) {}

        ~MallocBasedAllocator() override;

    public:
        void* alloc(std::size_t size) override;
        void free(void* addr) override;
        void print(std::ostringstream& out) override;

        malloc_based::AllocationStat& alloc_stat() {
            return stat;
        }

        void free_if(const std::function<bool(void* addr)>& function);
    private:
        details::SpinLock lock{};
        malloc_based::AllocationStat stat{}; //todo move to AllocInfo
        std::size_t max_size{};
        malloc_based::AllocInfo allocInfo;
    };
}
