#include <nanobench.h>

#include "gc/allocators/MallocBasedAllocator.h"
#include "gc/allocators/SemispacesAllocator.h"
#include <iostream>

void malloc_based_allocator_bench(ankerl::nanobench::Bench& bench) {
    gccpp::MallocBasedAllocator allocator(1024 * 1024 * 1280);
    auto fn = [&] {
        for(std::size_t i = 0; i < 50000; i++) {
            ankerl::nanobench::doNotOptimizeAway(allocator.alloc(std::rand() % 100));
        }
    };

    bench.run("malloc_based_allocator", fn);
}

void seq_allocator_bench(ankerl::nanobench::Bench& bench) {
    gccpp::SemispacesAllocator allocator(1024 * 1024 * 1280);
    auto fn = [&] {
        for(std::size_t i = 0; i < 50000; i++) {
            ankerl::nanobench::doNotOptimizeAway(allocator.alloc(std::rand() % 100));
        }
    };

    bench.run("semispaces_allocator", fn);
}

int main() {
    ankerl::nanobench::Bench b;
    b.minEpochIterations(110);
    malloc_based_allocator_bench(b);
    seq_allocator_bench(b);
}