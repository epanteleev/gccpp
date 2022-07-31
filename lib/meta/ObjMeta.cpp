#include "ObjMeta.h"

namespace tgc {
    void *ObjMeta::objPtr() const {
        return (void *) ((char*)this + sizeof(ObjMeta));
    }
}