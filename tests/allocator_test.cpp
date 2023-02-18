#include <gtest/gtest.h>
#include "gc/allocators/LinearAllocator.h"

TEST(allocation, creation) {
    gccpp::LinearAllocator allocator(64);

    auto first = allocator.alloc(24);
    ASSERT_NE(first, nullptr);
    ASSERT_TRUE(allocator.contains(first));

    ASSERT_EQ(reinterpret_cast<std::size_t>(first) % 8, 0);

    ASSERT_NE(allocator.alloc(22), nullptr);
    ASSERT_EQ(allocator.alloc(1), nullptr);
}

TEST(allocation, no_aligned_max_size) {
    gccpp::LinearAllocator allocator(60);

    auto first = allocator.alloc(24);
    ASSERT_NE(first, nullptr);
    ASSERT_TRUE(allocator.contains(first));
    ASSERT_EQ(reinterpret_cast<std::size_t>(first) % 8, 0);

    ASSERT_NE(allocator.alloc(22), nullptr);
    ASSERT_EQ(allocator.alloc(1), nullptr);
}

TEST(allocation, iterate) {
    gccpp::LinearAllocator allocator(128);

    allocator.alloc(24);
    allocator.alloc(22);
    auto third = allocator.alloc(32);
    ASSERT_NE(third, nullptr);
    auto forth = allocator.alloc(64);
    ASSERT_EQ(forth, nullptr);

    std::size_t objects = 0;
    auto fn = [&](gccpp::Header* h)-> void {
        ASSERT_EQ(h->magic, gccpp::LinearAllocator::MAGIC);
        objects += 1;
    };
    allocator.visit(fn);

    ASSERT_EQ(objects, 3);
}


TEST(allocation, iterate1) {
    gccpp::LinearAllocator allocator(64);

    std::size_t objects = 0;
    auto fn = [&](gccpp::Header* h)-> void {
        objects += 1;
    };
    allocator.visit(fn);
    ASSERT_EQ(objects, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
