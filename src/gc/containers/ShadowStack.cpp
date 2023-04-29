#include "ShadowStack.h"
#include <cstdio>
#include <exception>

namespace gccpp::details {
    ObjectPointer* ShadowStack::push(const gccpp::details::ObjectPointer& ptr) {
        stack.push(ptr);
        sp += 1;
        return &stack[sp - 1];
    }
}