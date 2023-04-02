#pragma once

#include "gc/containers/Enviroment.h"
#include "gc/GarbageCollected.h"
#include "gc/collectors/MarkAndSweepCollector.h"
#include "gc/operations/GCOperation.h"
#include "pointer/Oop.inline.h"
#include "Array.h"

template<gccpp::GarbageCollectedType T>
class Vector : public gccpp::GarbageCollected {
private:
    static constexpr float RESIZE_RATIO = 2;
    static constexpr std::size_t INITIAL_SIZE = 16;
public:
    explicit Vector(gccpp::Oop<Array<T>> _data):
            data(_data) {};

public:
    gccpp::Oop<T>& at(std::size_t idx) {
        return data->at(idx);
    }

    inline std::size_t length() noexcept {
        return count;
    }

    void push_back(gccpp::Oop<T> elem) {
        if (count == data->length()) {
            resize();
        }
        assert(count < data->length());
        data->at(count) = elem;
        count += 1;
    }

    void trace(gccpp::GCOperation *operation) noexcept override {
        operation->trace(data);
    }

private:
    void resize() noexcept {
        resize_static(gccpp::Oop<Vector>::from(this));
    }

public:
    static gccpp::Oop<Vector<T>> make() {
        auto& env = gccpp::Enviroment::context();
        auto array = Array<T>::make(INITIAL_SIZE);

        return env.alloc<Vector<T>>(array);
    }

    //bubble sort
    static void sort(gccpp::Oop<Vector> vec, std::function<bool(gccpp::Oop<T>, gccpp::Oop<T>)> comp) {
        auto length = vec->length();
        for (std::size_t i = 0; i < length - 1; i++) {
            for (std::size_t j = 0; j < length - i - 1; j++) {
                if (comp(vec->at(j), vec->at(j + 1))) {
                    auto tmp = vec->at(j);
                    vec->at(j) = vec->at(j + 1);
                    vec->at(j + 1) = tmp;
                }
            }
        }
    }

private:
    static void resize_static(gccpp::Oop<Vector> self) {
        gccpp::HandleMark hm;
        gccpp::Handle hm_self(self);

        std::size_t new_size = hm_self->data->length() * RESIZE_RATIO;
        auto new_array = Array<T>::make(new_size);
        if (!hm_self->data->copy_to(new_array)) {
            fprintf(stderr, "array copy error\n");
            std::terminate(); //todo
        }
        hm_self->data = new_array;
    }

private:
    std::size_t count{};
    gccpp::Oop<Array<T>> data;
};
