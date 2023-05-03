#pragma once
#include "gc/containers/Environment.h"
#include "gc/GarbageCollected.h"
#include "gc/operations/GCOperation.h"
#include "pointer/Oop.inline.h"

class String: public gccpp::GarbageCollected {
public:
    explicit String(std::size_t length):
            len(length) {}
public:
    [[nodiscard]]
    char peek(std::size_t idx) const {
        if (idx >= len) {
            fprintf(stderr, "String::peek(idx) error\n");
            std::terminate(); //todo
        }
        return data[idx];
    }

    [[nodiscard]]
    inline std::size_t length() const noexcept {
        return len;
    }

    [[nodiscard]]
    const char* cstr() const noexcept {
        return data;
    }

    [[nodiscard]]
    char* raw_data() noexcept {
        return data;
    }
public:
    bool operator!=(const String& other) {
        if (other.len != len) {
            return true;
        }
        return std::strncmp(data, other.data, len) != 0;
    }

public:
    void trace(gccpp::GCOperation *operation) noexcept override {
        (void)(operation);
    }

public:
    static gccpp::Oop<String> make(std::size_t length) {
        auto str = gccpp::Environment::context().raw_alloc<String>(length + 1);
        new(str.mw()) String(length);
        str->data[length] = '\0';
        return str;
    }

    static gccpp::Oop<String> make(std::string_view string) {
        auto length = string.length() + 1;
        auto str = gccpp::Environment::context().raw_alloc<String>(length);
        gccpp::Environment::init_object(str, length - 1);

        std::memcpy(str->data, string.data(), string.length());
        str->data[length - 1] = '\0';
        return str;
    }

private:
    std::size_t len;
    char data[1]{};
};

namespace std {
    template<>
    struct hash<String> {
        std::size_t operator()(String &k) const {
            std::size_t hash{};
            for(std::size_t i = 0; i < k.length(); i++) {
                hash += k.peek(i);
            }
            return hash;
        }
    };
}