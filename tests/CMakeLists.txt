include(../cmake/GoogleTest.cmake)

enable_testing()

add_executable(gc-unit-tests test.cpp)

target_link_libraries(gc-unit-tests PRIVATE libgc GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(gc-unit-tests)
