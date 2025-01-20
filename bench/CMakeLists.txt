cmake_minimum_required(VERSION 3.11)

include(../cmake/Nanobench.cmake)


add_executable(benchmarks allocation.cpp)
target_link_libraries(benchmarks nanobench libgc::libgc)
target_compile_features(benchmarks PRIVATE cxx_std_20)

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    target_link_libraries(Benchmarks Shlwapi)
endif ()