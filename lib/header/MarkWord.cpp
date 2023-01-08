#include "MarkWord.h"

namespace tgc {
    void *MarkWord::objPtr() const {
        return (void *) ((char*)this + sizeof(MarkWord));
    }
}