#include <gtest/gtest.h>
#include "gc/allocators/LinearAllocator.h"
#include "gc/allocators/MallocBasedAllocator.h"

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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}