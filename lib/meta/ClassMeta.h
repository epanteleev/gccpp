#pragma once

#include <cassert>
#include <memory>
#include <set>
#include <typeinfo>
#include <vector>
#include <atomic>
#include <shared_mutex>

#include <deque>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ObjMeta.h"

namespace tgc::details {
    class PtrBase;

    class IPtrEnumerator {
    public:
        virtual ~IPtrEnumerator() = default;

        virtual const PtrBase *getNext() = 0;

        void *operator new(size_t sz) {
            static char buf[255];
            assert(sz <= sizeof(buf));
            return buf;
        }

        void operator delete(void *) {}
    };

    class ClassMeta final {
    public:
        enum class State : unsigned char {
            Unregistered, Registered
        };
        enum class MemRequest {
            Alloc, Dctor, Dealloc, NewPtrEnumerator
        };
        using MemHandler = void *(*)(ClassMeta *cls, MemRequest r, void *param);
        using OffsetType = unsigned short;
        using SizeType = unsigned short;

    public:
        MemHandler memHandler = nullptr;
        std::vector<OffsetType> *subPtrOffsets = nullptr;
        State state = State::Unregistered;
        SizeType size = 0;

        std::shared_mutex mutex;

    public:
        ClassMeta() = default;

        ClassMeta(MemHandler h, SizeType sz) : memHandler(h), size(sz) {}

        ~ClassMeta() { delete subPtrOffsets; }
    };
}