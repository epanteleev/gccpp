include(../cmake/GoogleTest.cmake)

enable_testing()

set(TARGET_NAME gc-unit-tests)

add_executable(${TARGET_NAME} gc_test.cpp)
add_executable(allocator_test allocator_test.cpp)

target_link_libraries(${TARGET_NAME} PRIVATE libgc GTest::gtest GTest::gtest_main)

target_link_libraries(allocator_test PRIVATE libgc GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${TARGET_NAME})
