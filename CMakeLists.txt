cmake_minimum_required(VERSION 3.22)
project(cpp_garbage_collector)

set(CMAKE_CXX_STANDARD 20)

include_directories(lib)

add_library(libgc STATIC
        lib/pointer/GcPtr.h
        lib/pointer/ObjectPointer.h
        lib/pointer/PtrBase.cpp
        lib/header/MarkWord.h
        lib/header/MarkWord.cpp
        lib/pointer/GcRoot.cpp
        lib/pointer/GcRoot.h
        lib/gc/Gc.cpp
        lib/gc/Gc.h
        lib/gc/ShadowStack.cpp
        lib/gc/ShadowStack.h
        lib/gc/GcCollected.h
        lib/gc/operations/Mark.h
        lib/gc/Allocator.cpp
        lib/gc/Allocator.h
        lib/gc/operations/Sweep.h
        lib/gc/operations/GCOperation.h
        lib/gc/operations/Mark.cpp
        lib/gc/operations/Sweep.cpp
        lib/gc/Frame.h)

add_executable(cpp_garbage_collector main.cpp )
target_link_libraries(cpp_garbage_collector PRIVATE libgc)

add_subdirectory(tests)
