#pragma once
#include <gtest/gtest.h>
#include "gc/allocators/details/LinearAllocator.h"
#include "gc/allocators/MallocBasedAllocator.h"
#include "gc/allocators/details/FixedSizeAllocator.h"

TEST(fixedsize_allocation, allocation0) {
    gccpp::details::FixedSizeAllocator<64> allocator(64);
    ASSERT_EQ(allocator.allocation_count(), 0);

    auto first = allocator.alloc();
    ASSERT_NE(first, nullptr);
    ASSERT_TRUE(allocator.contains(first));
    ASSERT_EQ(allocator.allocation_count(), 1);

    allocator.visit([&](void* addr) {
        ASSERT_EQ(addr, first);
    });
}

TEST(fixedsize_allocation, allocation1) {
    gccpp::details::FixedSizeAllocator<8> allocator(64);
    std::vector<void*> vec;

    for (auto i = 0; i < 8; i++) {
        vec.emplace_back(allocator.alloc());
    }

    std::size_t idx = 0;
    allocator.visit([&](void* addr) {
        ASSERT_EQ(addr, vec[idx]);
        idx += 1;
    });

    ASSERT_EQ(allocator.allocation_count(), 8);
}

TEST(linear_allocation, creation) {
    gccpp::LinearAllocator allocator(64);

    auto first = allocator.alloc(24);
    ASSERT_NE(first, nullptr);
    ASSERT_TRUE(allocator.contains(first));

    ASSERT_EQ(reinterpret_cast<std::size_t>(first) % 8, 0);

    ASSERT_NE(allocator.alloc(22), nullptr);
    ASSERT_EQ(allocator.alloc(1), nullptr);
}

TEST(linear_allocation, no_aligned_max_size) {
    gccpp::LinearAllocator allocator(60);

    auto first = allocator.alloc(24);
    ASSERT_NE(first, nullptr);
    ASSERT_TRUE(allocator.contains(first));
    ASSERT_EQ(reinterpret_cast<std::size_t>(first) % 8, 0);

    ASSERT_NE(allocator.alloc(22), nullptr);
    ASSERT_EQ(allocator.alloc(1), nullptr);
}

TEST(linear_allocation, iterate) {
    gccpp::LinearAllocator allocator(128);

    allocator.alloc(24);
    allocator.alloc(22);
    auto third = allocator.alloc(32);
    ASSERT_NE(third, nullptr);
    auto forth = allocator.alloc(64);
    ASSERT_EQ(forth, nullptr);

    std::size_t objects = 0;
    auto fn = [&](gccpp::Chunk* h)-> void {
        ASSERT_EQ(h->magic, gccpp::LinearAllocator::MAGIC);
        objects += 1;
    };
    allocator.visit(fn);

    ASSERT_EQ(objects, 3);
}

TEST(allocation, iterate1) {
    gccpp::LinearAllocator allocator(64);

    std::size_t objects = 0;
    auto fn = [&](gccpp::Chunk* h)-> void {
        (void)(h);
        objects += 1;
    };
    allocator.visit(fn);
    ASSERT_EQ(objects, 0);
}

TEST(alloc_info, creation) {
    gccpp::malloc_based::AllocInfo info;
    auto one = new gccpp::malloc_based::Chunk;
    auto two = new gccpp::malloc_based::Chunk;
    auto three = new gccpp::malloc_based::Chunk;
    auto foo = new gccpp::malloc_based::Chunk;
    info.append(one);

    ASSERT_EQ(info.first, one);
    ASSERT_EQ(info.last, one);

    info.append(two);
    info.append(three);

    ASSERT_EQ(info.first, one);
    ASSERT_EQ(info.first->next, two);
    ASSERT_EQ(info.first->next->next, info.last);
    ASSERT_EQ(info.last, three);

    info.append(foo);
    ASSERT_EQ(info.first->next->next, three);
    ASSERT_EQ(info.first->next->next->next, foo);
    ASSERT_EQ(info.first->next->next->next, info.last);

    info.remove(three);
    ASSERT_EQ(info.first, one);
    ASSERT_EQ(info.first->next, two);
    ASSERT_EQ(info.first->next->next, info.last);
    ASSERT_EQ(info.last, foo);

    info.remove(one);
    ASSERT_EQ(info.first, two);
    ASSERT_EQ(info.first->next, info.last);
    ASSERT_EQ(info.last, foo);

    info.remove(foo);
    ASSERT_EQ(info.first, two);
    ASSERT_EQ(info.last, two);

    info.remove(two);
    ASSERT_EQ(info.first, nullptr);
    ASSERT_EQ(info.last, nullptr);

    delete one;
    delete two;
    delete three;
    delete foo;
}

TEST(markword, set_get_fields) {
    auto ptr = new int[10];
    gccpp::MarkWord mw;
    mw.set_color(gccpp::MarkWord::Color::Black);
    ASSERT_EQ(mw.color(), gccpp::MarkWord::Color::Black);

    mw.set_forwarding_ptr(ptr);
    ASSERT_EQ(mw.color(), gccpp::MarkWord::Color::Black);
    ASSERT_EQ(mw.forwarding_ptr(), ptr);
    delete[] ptr;
}