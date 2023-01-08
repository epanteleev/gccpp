#include "ShadowStack.h"
#include <cstdio>

namespace tgc::details {
    ObjectPointer* ShadowStack::push(const tgc::details::ObjectPointer& ptr) {
        if (sp == MAX_STACK_SIZE) {
            printf("Stack overflow\n");
            std::terminate();
        }
        stack[sp] = ptr; // Todo write barrier???
        auto pos = &stack[sp];
        sp += 1;
        return pos;
    }
}