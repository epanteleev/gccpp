#pragma once
#include "meta/ObjMeta.h"

namespace tgc::details {

    class PtrBase {
    public:
        PtrBase() = default;
        ~PtrBase() = default;

        PtrBase(ObjMeta* ptr) : p(ptr) {}

    public:
        ObjMeta* meta() const noexcept {
            return p; //Todo
        }

    public:
        void* ptr() const {
            return p->objPtr();
        }

        bool is_Root() const {
            return isRoot == 1;
        }

        int idx() const {
            return index;
        }
    public:
        ObjMeta* p{};
        mutable unsigned int isRoot: 1{};
        unsigned int index: 31{};
    };
}