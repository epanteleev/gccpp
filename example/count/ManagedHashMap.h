#pragma once

#include "gc/containers/Environment.h"
#include "gc/GarbageCollected.h"
#include "gc/operations/GCOperation.h"
#include "pointer/Oop.inline.h"
#include "struct/Array.h"

namespace managed {

    template<typename T>
    concept Hashable = requires(T a) {
        { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
    };

    template<typename T>
    concept Ne =
    requires(T a, T b) {
        { a != b } -> std::convertible_to<bool>;
    };

    template<typename T>
    concept HashMapKeyType = gccpp::GarbageCollectedType<T> && Ne<T> && Hashable<T>;

    template<HashMapKeyType Key, gccpp::GarbageCollectedType Value>
    class HashMap;

    template<HashMapKeyType Key, gccpp::GarbageCollectedType Value>
    class HashMapIterator: public gccpp::GarbageCollected {
    public:
        using HashMapT = HashMap<Key, Value>;

    private:
        void advance();

    public:
        explicit HashMapIterator(gccpp::Oop<HashMapT> _hm);

    public:
        bool hasNext() {
            return next != nullptr;
        }

        gccpp::Oop<typename HashMapT::Node> nextNode();
    public:
        void trace(gccpp::GCOperation *operation) noexcept override;

    private:
        std::size_t bucket_index{};
        gccpp::Oop<typename HashMapT::Node> current{};
        gccpp::Oop<typename HashMapT::Node> next{};
        gccpp::Oop<HashMapT> hashMap{};
    };


    template<HashMapKeyType Key, gccpp::GarbageCollectedType Value>
    class HashMap : public gccpp::GarbageCollected {
    public:
        friend class HashMapIterator<Key, Value>;
    public:
        static constexpr std::size_t ARRAY_SIZE = 1024;

    private:
        struct Node : public gccpp::GarbageCollected {
        public:
            Node(gccpp::Oop<Key> _key, gccpp::Oop<Value> _value, std::size_t _hash) :
                    key(_key),
                    value(_value),
                    hash(_hash) {}

            void trace(gccpp::GCOperation *operation) noexcept override {
                operation->trace(key);
                operation->trace(value);
                operation->trace(next);
            }
        public:
            gccpp::Oop<Key>   key{};
            gccpp::Oop<Value> value{};
            std::size_t       hash{};
            gccpp::Oop<Node>  next{};
        };

    private:
        static gccpp::Oop<Node> makeNode(gccpp::Oop<Key> _key, gccpp::Oop<Value> _value, std::size_t _hash) {
            return gccpp::Environment::context().alloc<Node>(_key, _value, _hash);
        }

    public:
        explicit HashMap(gccpp::Oop<Array<Node>> _table):
            table(_table) {}

    public:
        void put(gccpp::Oop<Key> key, gccpp::Oop<Value> value) {
            gccpp::HandleMark hm;
            gccpp::Handle h_key(key);
            gccpp::Handle h_value(value);
            gccpp::Handle h_table(table);

            assert(h_key);
            assert(h_value);
            size += 1;

            auto hash = std::hash<Key>()(*key);
            auto new_node = makeNode(h_key, h_value, hash);
            auto node = h_table->at((h_table->length() - 1) & hash);
            if (node == nullptr) {
                h_table->at((h_table->length() - 1) & hash) = new_node;
                return;
            }
            auto current = node;
            while (node != nullptr) {
                current = node;
                node = node->next;
            }
            current->next = new_node;
        }

        gccpp::Oop<Value> get(gccpp::Oop<Key> key) {
            if (key == nullptr) {
                return nullptr;
            }
            auto hash = std::hash<Key>()(*key);
            auto node = table->at((table->length() - 1) & hash);
            while (node != nullptr) {
                if (node->hash == hash && !((*node->key) != (*key))) {
                    return node->value;
                }
                node = node->next;
            }
            return nullptr;
        }

        gccpp::Oop<HashMapIterator<Key, Value>> iterator() {
            auto oop = gccpp::Oop<HashMap<Key,Value>>::from(this);
            return gccpp::Environment::context().alloc<HashMapIterator<Key, Value>>(oop);
        }
    public:
        void trace(gccpp::GCOperation *operation) noexcept override {
            operation->trace(table);
        }
    public:
        static gccpp::Oop<HashMap<Key, Value>> make() {
            auto& ctx = gccpp::Environment::context();
            gccpp::HandleMark hm;
            gccpp::Handle tab(Array<Node>::make(ARRAY_SIZE));

            return ctx.alloc<HashMap<Key, Value>>(tab);
        }
    private:
        gccpp::Oop<Array<Node>> table;
        std::size_t size{};
    };

    template<HashMapKeyType Key, gccpp::GarbageCollectedType Value>
    void HashMapIterator<Key, Value>::advance() {
        if (hashMap->table == nullptr || hashMap->size == 0) {
            return;
        }
        while (bucket_index < hashMap->table->length()) {
            next = hashMap->table->at(bucket_index);
            bucket_index += 1;
            if (next != nullptr) {
                return;
            }
        }
    }

    template<HashMapKeyType Key, gccpp::GarbageCollectedType Value>
    HashMapIterator<Key, Value>::HashMapIterator(gccpp::Oop<HashMapT> _hm) :
            hashMap(_hm) {
        advance();
    }

    template<HashMapKeyType Key, gccpp::GarbageCollectedType Value>
    gccpp::Oop<typename HashMap<Key, Value>::Node> HashMapIterator<Key, Value>::nextNode() {
        auto e = next;
        next = e->next;
        if (next == nullptr) {
            advance();
        }
        return e;
    }

    template<HashMapKeyType Key, gccpp::GarbageCollectedType Value>
    void HashMapIterator<Key, Value>::trace(gccpp::GCOperation *operation) noexcept {
        operation->trace(current);
        operation->trace(next);
        operation->trace(hashMap);
    }
}
