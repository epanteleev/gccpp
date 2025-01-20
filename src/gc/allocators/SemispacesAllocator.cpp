#include "SemispacesAllocator.h"

#include <cstdio>
#include <sstream>

namespace gccpp {
    SemispacesAllocator::~SemispacesAllocator() {
        delete active_space;
        delete free_space;
    }

    void *SemispacesAllocator::alloc(std::size_t size) noexcept {
        if (size <= 32) {
            auto addr = small_allocator32.alloc();
            if (addr != nullptr) {
                return addr;
            }
        }
        else if (size < 64) {
            auto addr = small_allocator64.alloc();
            if (addr != nullptr) {
                return addr;
            }
        }
        return active_space->alloc(size);
    }

    void SemispacesAllocator::print(std::ostringstream &out) {
        out << "-- active space --" << "\n";
        active_space->print(out);
        out << "-- free space --" << "\n";
        free_space->print(out);

        out << "-- small space 32b --" << "\n";
        small_allocator32.print(out);

        out << "-- small space 64b --" << "\n";
        small_allocator64.print(out);
    }
}