#include <gtest/gtest.h>

#include "gc_multithread_test.h"
#include "gc_test.h"
#include "allocator_test.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
