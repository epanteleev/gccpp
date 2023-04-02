#pragma once
#include <cstdint>
#include <unistd.h>

class Page final {
public:
    static void* alloc(std::size_t size) noexcept;
    static void* realloc(void *old_addr, std::size_t old_size, std::size_t new_size) noexcept;
    static void free(void *old_addr, std::size_t size) noexcept;
};