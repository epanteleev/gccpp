include(../cmake/GoogleTest.cmake)

enable_testing()

set(TARGET_NAME gc-unit-tests)

set(TEST_SRC
    src/gc_test.h
    src/allocator_test.h
    src/gc_multithread_test.h
    utils/GCTest.h
    utils/Objects.h
    utils/Objects.cpp
)
add_executable(${TARGET_NAME} src/test_runner.cpp ${TEST_SRC})
target_link_libraries(${TARGET_NAME} PRIVATE libgc::libgc GTest::gtest GTest::gtest_main)
target_compile_features(${TARGET_NAME} PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(${TARGET_NAME})