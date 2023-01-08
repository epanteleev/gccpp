cmake_minimum_required(VERSION 3.22)
project(cpp_garbage_collector)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_library(libgc STATIC
        src/pointer/GcPtr.h
        src/pointer/ObjectPointer.h
        src/header/MarkWord.h
        src/pointer/GcRoot.h
        src/gc/collector/MarkAndSweepCollector.cpp
        src/gc/collector/MarkAndSweepCollector.h
        src/gc/ShadowStack.cpp
        src/gc/ShadowStack.h
        src/gc/GCCollected.h
        src/gc/operations/Mark.h
        src/gc/Allocator.cpp
        src/gc/Allocator.h
        src/gc/operations/Sweep.h
        src/gc/operations/GCOperation.h
        src/gc/operations/Mark.cpp
        src/gc/operations/Sweep.cpp
        src/gc/Frame.h
)

add_executable(cpp_garbage_collector main.cpp )
target_link_libraries(cpp_garbage_collector PRIVATE libgc)

add_subdirectory(tests)
