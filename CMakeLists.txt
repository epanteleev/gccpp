cmake_minimum_required(VERSION 3.22)
project(cpp_garbage_collector)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address -fsanitize=undefined ")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -Wall -Wextra -Wpedantic -Werror ")

add_library(libgc STATIC
    src/pointer/Oop.h
    src/pointer/ObjectPointer.h
    src/gc/MarkWord.h
    src/gc/collectors/MarkAndSweepCollector.cpp
    src/gc/collectors/MarkAndSweepCollector.h
    src/gc/containers/ShadowStack.cpp
    src/gc/containers/ShadowStack.h
    src/gc/GarbageCollected.h
    src/gc/operations/Mark.h
    src/gc/allocators/Allocator.h
    src/gc/operations/Sweep.h
    src/gc/operations/GCOperation.h
    src/gc/operations/Mark.cpp
    src/gc/operations/Sweep.cpp
    src/pointer/Handle.h
    src/gc/operations/Reallocate.cpp
    src/gc/operations/Reallocate.h
    src/gc/collectors/MarkAndCompactCollector.cpp
    src/gc/collectors/MarkAndCompactCollector.h
    src/gc/collectors/BasicCollector.h
    src/gc/allocators/MallocBasedAllocator.cpp
    src/gc/allocators/MallocBasedAllocator.h
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
    src/gc/containers/Enviroment.h
    src/gc/collectors/BasicCollector.cpp
    src/gc/containers/Worker.cpp
    src/gc/containers/Enviroment.cpp
    src/gc/containers/ThreadLock.h
    src/gc/containers/ThreadLock.cpp
    src/pointer/Handle.cpp
    src/pointer/Oop.inline.h
    src/gc/containers/Buffer.h
    src/gc/collectors/IncrementalMarkAndSweep.h
    src/gc/collectors/IncrementalMarkAndSweep.cpp
    src/gc/fwd.h
    src/gc/containers/SpinLock.h
    src/gc/operations/MarkIncrementally.h
    src/gc/operations/MarkIncrementally.cpp
    src/gc/containers/Worker.h
    src/gc/containers/Enviroment.h
    src/gc/collectors/BasicCollector.cpp
    src/gc/containers/Worker.cpp
    src/gc/containers/Memory.h
    src/gc/containers/WorkerState.h
    src/gc/containers/ThreadSafepointState.cpp
    src/gc/containers/ThreadSafepointState.h
    src/gc/containers/WaitBarrier.h
    src/gc/containers/Page.cpp
    src/gc/containers/Page.h
        )


set(DATA_STRUCTURES
    src/struct/String.h
    src/struct/Array.h
    src/struct/Vector.h
    src/struct/SizeT.h
    src/struct/Pair.h
    example/count/UnmanagedHashMap.h
    example/count/ManagedHashMap.h
    src/macros.h
)

add_executable(count example/count/count.cpp ${DATA_STRUCTURES})
target_link_libraries(count PRIVATE libgc)

add_executable(cpp_garbage_collector main.cpp )
target_link_libraries(cpp_garbage_collector PRIVATE libgc)

add_subdirectory(tests)
add_subdirectory(bench)