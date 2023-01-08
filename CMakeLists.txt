cmake_minimum_required(VERSION 3.22)
project(cpp_garbage_collector)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_library(libgc STATIC
        src/pointer/GcPtr.h
        src/pointer/ObjectPointer.h
        src/header/MarkWord.h
        src/pointer/GcRoot.h
        src/gc/collectors/MarkAndSweepCollector.cpp
        src/gc/collectors/MarkAndSweepCollector.h
        src/gc/ShadowStack.cpp
        src/gc/ShadowStack.h
        src/gc/GCCollected.h
        src/gc/operations/Mark.h
        src/gc/allocators/Allocator.h
        src/gc/operations/Sweep.h
        src/gc/operations/GCOperation.h
        src/gc/operations/Mark.cpp
        src/gc/operations/Sweep.cpp
        src/gc/Frame.h
        src/gc/operations/Compact.cpp
        src/gc/operations/Compact.h
        src/gc/collectors/MarkAndCompactCollector.cpp
        src/gc/collectors/MarkAndCompactCollector.h
        src/gc/collectors/GC.h
        src/gc/allocators/NaiveAllocator.cpp
        src/gc/allocators/NaiveAllocator.h
)

set(CMAKE_CXX_FLAGS "-fno-strict-aliasing")
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -Wall -Wextra -Wpedantic -Werror -fno-strict-aliasing")

add_executable(cpp_garbage_collector main.cpp )
target_link_libraries(cpp_garbage_collector PRIVATE libgc)

add_subdirectory(tests)