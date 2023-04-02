#include "SemispacesAllocator.h"

#include <cstdio>
#include <exception>
#include <sstream>

namespace gccpp {
    SemispacesAllocator::~SemispacesAllocator() {
        delete active_space;
        delete free_space;
    }

    void *SemispacesAllocator::alloc(std::size_t size) {
        return active_space->alloc(size);
    }

    void SemispacesAllocator::free(void *addr) {
        (void)(addr); //todo
        printf("SemispacesAllocator::free is unreachable.\n");
        std::terminate();
    }

    void SemispacesAllocator::print(std::ostringstream &out) {
        out << "-- active space --" << "\n";
        active_space->print(out);
        out << "-- free space --" << "\n";
        free_space->print(out);
    }
}