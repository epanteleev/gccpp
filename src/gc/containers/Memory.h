#pragma once

namespace gccpp::details::mem {
    inline static void write_barrier() {
        asm volatile("": : :"memory");
    }
}
