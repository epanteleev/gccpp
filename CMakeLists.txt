cmake_minimum_required(VERSION 3.22)
project(cpp_garbage_collector)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_library(libgc STATIC
        src/pointer/GcPtr.h
        src/pointer/ObjectPointer.h
        src/header/MarkWord.h
        src/gc/collectors/MarkAndSweepCollector.cpp
        src/gc/collectors/MarkAndSweepCollector.h
        src/gc/containers/ShadowStack.cpp
        src/gc/containers/ShadowStack.h
        src/gc/GCCollected.h
        src/gc/operations/Mark.h
        src/gc/allocators/Allocator.h
        src/gc/operations/Sweep.h
        src/gc/operations/GCOperation.h
        src/gc/operations/Mark.cpp
        src/gc/operations/Sweep.cpp
        src/gc/Handle.h
        src/gc/operations/Compact.cpp
        src/gc/operations/Compact.h
        src/gc/collectors/MarkAndCompactCollector.cpp
        src/gc/collectors/MarkAndCompactCollector.h
        src/gc/collectors/BasicCollector.h
        src/gc/allocators/NaiveAllocator.cpp
        src/gc/allocators/NaiveAllocator.h
        src/gc/allocators/LinearAllocator.cpp
        src/gc/allocators/LinearAllocator.h
        src/gc/allocators/SemispacesAllocator.cpp
        src/gc/allocators/SemispacesAllocator.h
        src/gc/operations/UpdateReference.cpp
        src/gc/operations/UpdateReference.h
        src/gc/operations/Relocate.cpp
        src/gc/operations/Relocate.h
        src/gc/containers/ThreadsStacks.cpp
        src/gc/containers/ThreadsStacks.h
        src/gc/containers/Worker.h
        src/gc/containers/GlobalCtx.h
        src/gc/collectors/BasicCollector.cpp
        src/gc/containers/Worker.cpp
        src/gc/containers/GlobalCtx.cpp
        src/gc/containers/ThreadLock.h src/gc/containers/ThreadLock.cpp src/gc/Handle.cpp src/pointer/GcPtrImpl.h)

set(CMAKE_CXX_FLAGS "-fno-strict-aliasing -fsanitize=address")
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -Wall -Wextra -Wpedantic -Werror -fno-strict-aliasing")

add_executable(cpp_garbage_collector main.cpp src/gc/containers/Worker.h src/gc/containers/GlobalCtx.h src/gc/collectors/BasicCollector.cpp src/gc/containers/Worker.cpp)
target_link_libraries(cpp_garbage_collector PRIVATE libgc)

add_subdirectory(tests)