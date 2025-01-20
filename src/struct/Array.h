#pragma once
#include "gc/containers/Environment.h"
#include "gc/GarbageCollected.h"
#include "gc/operations/GCOperation.h"
#include "pointer/Oop.inline.h"

template<gccpp::GarbageCollectedType T>
class Array final : public gccpp::GarbageCollected {
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

    [[nodiscard]]
    std::size_t length() const noexcept {
        return len;
    }

    void trace(gccpp::GCOperation *operation) noexcept override {
        for (std::size_t idx = 0; idx < len; idx++) {
            operation->trace(data[idx]);
        }
    }

    [[nodiscard]]
    bool copy_to(gccpp::Oop<Array> other) {
        if (len > other->length()) {
            return false;
        }
        void* to_data = other.template content<Array>()->data;
        auto bytes = len * gccpp::Oop<Array>::sizeOf();
        std::memcpy(to_data, static_cast<void *>(data), bytes);
        return true;
    }
public:
    static gccpp::Oop<Array<T>> make(std::size_t length) {
        auto size_in_bytes = length * gccpp::Oop<Array>::sizeOf();
        auto array = gccpp::Environment::context().raw_alloc<Array>(size_in_bytes);
        gccpp::Environment::init_object(array, length);
        return array;
    }

private:
    std::size_t len;
    gccpp::Oop<T> data[1];
};
