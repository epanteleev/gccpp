#pragma once

#include "pointer/Oop.h"
#include "Handle.h"
#include "gc/containers/Enviroment.h"

namespace gccpp {
    template<typename T>
    Oop<T>::Oop(const Handle<T> &r) : ObjectPointer(r.oop()) {}

    template<typename T>
    Oop<T> &Oop<T>::operator=(Handle<T> &r) noexcept {
        Enviroment::collector()->write_barrier(this, r.oop());
        return *this;
    }

    template<typename T>
    bool Oop<T>::operator==(const Handle<T> &r) const {
        return p == r.oop().p;
    }

    template<typename T>
    bool Oop<T>::operator!=(const Handle<T> &r) const {
        return p != r.oop().p;
    }

    template<typename T>
    Oop<T> &Oop<T>::operator=(const Oop<T> &r) {
        if (this == &r) {
            return *this;
        }
        p = r.p;
        //Enviroment::collector()->write_barrier(this, r);
        return *this;
    }

    template<typename T>
    Oop<T> &Oop<T>::operator=(Oop &&r) noexcept {
       // Enviroment::collector()->write_barrier(this, r);
        p = r.p;
        r.p = nullptr;
        return *this;
    }

    template<typename T>
    typename Oop<T>::pointer Oop<T>::operator->() const {
        return static_cast<pointer>(p->object());
    }

    template<typename T>
    bool Oop<T>::operator!=(const Oop &r) const {
        return p != r.p;
    }

    template<typename T>
    bool Oop<T>::operator==(const Oop &r) const {
        return p == r.p;
    }

    template<typename T>
    Oop<T>::operator bool() const {
        return p != nullptr;
    }

    template<typename T>
    T &Oop<T>::operator*() const {
        return *static_cast<pointer>(p->object());
    }

    template<typename T>
    Handle<T> &Handle<T>::operator=(Oop<T> ptr) {
        //Enviroment::collector()->write_barrier(oop_ptr, ptr);
        *oop_ptr = ptr;
        return *this;
    }

    template<typename T>
    Handle<T> &Handle<T>::operator=(const Handle<T>& ptr) {
        *oop_ptr = *ptr.oop_ptr;
        return *this;
    }

    template<typename T>
    Handle<T>::operator bool() const {
        return (*oop_ptr).mw() != nullptr; //Todo read bar??
    }

    template<typename T>
    typename Handle<T>::pointer Handle<T>::operator->() const {
        return oop_ptr->content<T>();
    }

    template<typename T>
    typename Handle<T>::reference Handle<T>::operator*() const {
        return oop_ptr->content<T>();
    }

    template<typename T>
    Handle<T>::Handle(const details::ObjectPointer &oop) :
            oop_ptr(details::ThreadsStacks::stack_of_current_thread().push(oop)) {}

    template<typename T>
    Handle<T>::Handle(const Oop<T> &oop) :
            oop_ptr(details::ThreadsStacks::stack_of_current_thread().push(oop)) {}

    template<typename T>
    details::ObjectPointer Handle<T>::oop() const {
        return *oop_ptr; // Todo need read bar??
    }
}
