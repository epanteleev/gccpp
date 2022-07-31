cmake_minimum_required(VERSION 3.22)
project(cpp_garbage_collector)

set(CMAKE_CXX_STANDARD 20)

include_directories(lib)

add_library(libgc STATIC
        lib/pointer/GcPtr.h
        lib/pointer/GcPtr.cpp
        lib/pointer/PtrBase.h
        lib/pointer/PtrBase.cpp
        lib/meta/ObjMeta.h
        lib/meta/ObjMeta.cpp
        lib/meta/ClassMeta.h
        lib/meta/ClassMeta.cpp
        lib/pointer/GcRoot.cpp lib/pointer/GcRoot.h)

add_executable(cpp_garbage_collector main.cpp lib/Gc.cpp lib/Gc.h lib/ShadowStack.cpp lib/ShadowStack.h)
target_link_libraries(cpp_garbage_collector PRIVATE libgc)

add_subdirectory(tests)
