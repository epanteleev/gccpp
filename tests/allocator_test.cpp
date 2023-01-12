#include <gtest/gtest.h>
#include "gc/allocators/LinearAllocator.h"

TEST(allocation, creation) {
    gccpp::LinearAllocator allocator(64);

    ASSERT_NE(allocator.alloc(32), nullptr);
    ASSERT_NE(allocator.alloc(30), nullptr);
    ASSERT_EQ(allocator.alloc(1), nullptr);
}

TEST(allocation, no_aligned_max_size) {
    gccpp::LinearAllocator allocator(60);

    ASSERT_NE(allocator.alloc(32), nullptr);
    ASSERT_NE(allocator.alloc(30), nullptr);
    ASSERT_EQ(allocator.alloc(1), nullptr);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
