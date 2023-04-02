#pragma once
#include <concepts>
#include <typeindex>
#include <array>

namespace unmanaged {

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
    concept HashMapKeyType = Ne<T> && Hashable<T>;


    template<HashMapKeyType Key, typename Value>
    class HashMap;

    template<HashMapKeyType Key, typename Value>
    class HashMapIterator {
    public:
        using HashMapT = HashMap<Key, Value>;

    private:
        void advance();

    public:
        explicit HashMapIterator(HashMapT* _hm);

    public:
        bool hasNext() {
            return next != nullptr;
        }

        typename HashMapT::Node* nextNode();

    private:
        std::size_t bucket_index{};
        typename HashMapT::Node* current{};
        typename HashMapT::Node* next{};
        HashMapT* hashMap{};
    };

    template<HashMapKeyType Key, typename Value>
    class HashMap final {
    public:
        friend class HashMapIterator<Key, Value>;
    public:
        static constexpr std::size_t ARRAY_SIZE = 1024;

    private:
        struct Node {
        public:
            Node(Key* _key, Value* _value, std::size_t _hash) :
                    key(_key),
                    value(_value),
                    hash(_hash) {}

        public:
            Key*   key{};
            Value* value{};
            std::size_t hash{};
            Node*  next{};
        };

    private:
        static Node* makeNode(Key* _key, Value* _value, std::size_t _hash) {
            return new Node(_key, _value, _hash);
        }

    public:
        explicit HashMap(std::array<Node*, ARRAY_SIZE>* _table):
                table(_table) {}

    public:
        void put(Key* key, Value* value) {
            assert(key);
            assert(value);
            size += 1;

            auto hash = std::hash<Key>()(*key);
            auto new_node = makeNode(key, value, hash);
            Node* node = table->at((table->size() - 1) & hash);
            if (node == nullptr) {
                table->at((table->size() - 1) & hash) = new_node;
                return;
            }
            auto current = node;
            while (node != nullptr) {
                current = node;
                node = node->next;
            }
            current->next = new_node;
        }

        Value* get(Key* key) {
            if (key == nullptr) {
                return nullptr;
            }
            auto hash = std::hash<Key>()(*key);
            Node* node = table->at((table->size() - 1) & hash);
            if (node == nullptr) {
                return nullptr;
            }

            while (node->hash != hash || (*node->key) != (*key)) {
                node = node->next;
                if (node == nullptr) {
                    return nullptr;
                }
            }
            return node->value;
        }

        HashMapIterator<Key, Value>* iterator() {
            return new HashMapIterator<Key, Value>(this);
        }
    public:
        static HashMap<Key, Value>* make() {
            auto arr = new std::array<Node*, ARRAY_SIZE>();

            return new HashMap<Key, Value>(arr);
        }
    private:
        std::array<Node*, ARRAY_SIZE>* table;
        std::size_t size{};
    };

    template<HashMapKeyType Key, typename Value>
    void HashMapIterator<Key, Value>::advance() {
        if (hashMap->table == nullptr || hashMap->size == 0) {
            return;
        }
        while (bucket_index < hashMap->table->size()) {
            next = hashMap->table->at(bucket_index);
            bucket_index += 1;
            if (next != nullptr) {
                return;
            }
        }
    }

    template<HashMapKeyType Key, typename Value>
    HashMapIterator<Key, Value>::HashMapIterator(HashMapT* _hm) :
            hashMap(_hm) {
        advance();
    }

    template<HashMapKeyType Key, typename Value>
    typename HashMap<Key, Value>::Node* HashMapIterator<Key, Value>::nextNode() {
        auto e = next;
        next = e->next;
        if (next == nullptr) {
            advance();
        }
        return e;
    }
}
