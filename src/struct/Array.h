#pragma once
#include "gc/containers/Enviroment.h"
#include "gc/GarbageCollected.h"
#include "gc/operations/GCOperation.h"
#include "pointer/Oop.inline.h"

template<gccpp::GarbageCollectedType T>
class Array : public gccpp::GarbageCollected {
public:
    explicit Array(std::size_t length):
            len(length) {}
public:
    gccpp::Oop<T>& at(std::size_t idx) {
        if (idx >= len) {
            fprintf(stderr, "Array::at(idx): out of bound idx=%lu, length=%lu\n", idx, len);
            std::terminate(); //todo
        }
        return data[idx];
    }

    inline std::size_t length() noexcept {
        return len;
    }

    void trace(gccpp::GCOperation *operation) noexcept override {
        for (std::size_t idx = 0; idx < len; idx++) {
            operation->trace(data[idx]);
        }
    }

    [[nodiscard]]
    bool copy_to(gccpp::Oop<Array<T>> other) {
        if (len > other->length()) {
            return false;
        }
        auto to_data = other.template content<Array<T>>()->data;
        auto bytes = len * gccpp::Oop<Array<T>>::sizeOf();
        std::memcpy(to_data, data, bytes);
        return true;
    }
public:
    static gccpp::Oop<Array<T>> make(std::size_t length) {
        auto size_in_bytes = length * gccpp::Oop<Array<T>>::sizeOf();
        auto array = gccpp::Enviroment::context().raw_alloc<Array<T>>(size_in_bytes);
        gccpp::Enviroment::init_object(array, length);
        return array;
    }

private:
    std::size_t len;
    gccpp::Oop<T> data[1];
};
