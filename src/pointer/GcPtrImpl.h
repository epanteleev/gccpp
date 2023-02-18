#pragma once

#include "pointer/GcPtr.h"
#include "gc/Handle.h"

namespace gccpp {
    template<typename T>
    GcPtr<T>::GcPtr(Handle<T> &r) : ObjectPointer(r.oop()) {}

    template<typename T>
    GcPtr<T> &GcPtr<T>::operator=(Handle<T> &r) noexcept {
        p = r.oop().p;
        return *this;
    }

    template<typename T>
    bool GcPtr<T>::operator==(const Handle<T> &r) const {
        return p == r.oop().p;
    }

    template<typename T>
    bool GcPtr<T>::operator!=(const Handle<T> &r) const {
        return p != r.oop().p;
    }
}
