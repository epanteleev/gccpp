#include "gc/containers/Page.h"
#include <sys/mman.h>
#include <cstdio>
#include <exception>
#include <cassert>

void *Page::alloc(std::size_t size) noexcept {
    void* page = mmap(nullptr, size, PROT_READ | PROT_WRITE | PROT_EXEC,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) {
        std::fprintf(stderr, "mmap failed: size=%zul\n", size);
        std::terminate();
    }
    return page;
}

void *Page::realloc(void *old_addr, std::size_t old_size, std::size_t new_size) noexcept {
    void *p = mremap(old_addr, old_size, new_size, MREMAP_MAYMOVE);
    if (p == MAP_FAILED) {
        std::fprintf(stderr, "mremap failed: old_addr=%p, old_size=%zu, new_size=%zu", old_addr, old_size, new_size);
        perror(" cause: ");
        std::terminate();
    }
    return p;
}

void Page::free(void *old_addr, std::size_t size) noexcept {
    int ret = munmap(old_addr, size);
    if (ret == -1) {
        std::fprintf(stderr, "munmap failed: old_addr=%p, old_size=%zul\n", old_addr, size);
        std::terminate();
    }
}

