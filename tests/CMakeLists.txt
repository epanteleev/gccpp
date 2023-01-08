include(../cmake/GoogleTest.cmake)

enable_testing()

set(TARGET_NAME gc-unit-tests)

add_executable(${TARGET_NAME} test.cpp)

target_link_libraries(${TARGET_NAME} PRIVATE libgc GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${TARGET_NAME})
