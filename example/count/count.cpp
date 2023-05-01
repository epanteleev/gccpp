#include "gc/collectors/MarkAndCompactCollector.h"
#include <iostream>
#include <fstream>
#include <ranges>
#include "struct/String.h"
#include "ManagedHashMap.h"
#include "UnmanagedHashMap.h"

#include "struct/SizeT.h"
#include "struct/Vector.h"
#include "struct/Pair.h"

namespace managed {

    void update_value(gccpp::Handle<HashMap<String, SizeT>>& hashMap, gccpp::Oop<String> string) {
        auto value = hashMap->get(string);

        if (value == nullptr) {
            gccpp::HandleMark hm;
            gccpp::Handle str(string);

            auto sizet = SizeT::make(1);
            hashMap->put(str, sizet);
        } else {
            value->store(value->value() + 1);
        }
    }

    gccpp::Oop<HashMap<String, SizeT>> parse_file(std::string_view buffer) {
        auto& env = gccpp::Enviroment::context();
        gccpp::HandleMark hm;
        gccpp::Handle hashMap(HashMap<String, SizeT>::make());

        auto pos = buffer.begin();
        while (pos != buffer.end()) {
            env.safepoint();
            if (std::isalnum(*pos)) {
                auto word_begin = pos;

                while ((std::isalnum(*pos) || std::ispunct(*pos)) && pos != buffer.end()) {
                    pos++;
                }
                std::string_view substr(word_begin, pos);
                update_value(hashMap, String::make(substr));
                continue;
            }
            pos++;
        }
        return hashMap;
    }

    void count(std::string_view buffer) {
        auto &env = gccpp::Enviroment::context();
        gccpp::ThreadEnv _th(env);
        gccpp::HandleMark hm;

        gccpp::Handle hashMap(parse_file(buffer));

        gccpp::Handle iter(hashMap->iterator());
        gccpp::Handle vector(Vector<Pair<String, SizeT>>::make());

        std::size_t words_count{};
        while(iter->hasNext()) {
            gccpp::HandleMark _hm1;
            gccpp::Handle curr(iter->nextNode());
            words_count += curr->value->value();
            auto pair = Pair<String, SizeT>::make(curr->key, curr->value);
            vector->push_back(pair);
        }

        env.unmanaged_context([&]() {
            std::cout << "Words: " << words_count << std::endl;
        });

        for (std::size_t i = 0; i < vector->length(); i++) {
            gccpp::HandleMark _hm1;
            gccpp::Handle elem(vector->at(i));

            std::cout << elem->first->cstr() <<" : " << elem->second->value() << std::endl;
            env.safepoint();
        }

        env.safepoint();
    }
}

namespace unmanaged {

    using Map = HashMap<std::string, std::size_t>;

    void update_value(Map* hashMap, std::string* string) {
        auto value = hashMap->get(string);

        if (value == nullptr) {
            hashMap->put(string, new std::size_t(1));
        } else {
            *value = *value + 1;
            delete string;
        }
    }

    Map* parse_file(std::string_view buffer) {
        auto hashMap = HashMap<std::string, std::size_t>::make();

        auto pos = buffer.begin();
        while (pos != buffer.end()) {
            if (std::isalnum(*pos)) {
                auto word_begin = pos;

                while ((std::isalnum(*pos) || std::ispunct(*pos)) && pos != buffer.end()) {
                    pos++;
                }
                std::string_view substr(word_begin, pos);
                update_value(hashMap, new std::string(substr.begin(), substr.end()));
                continue;
            }
            pos++;
        }
        return hashMap;
    }

    void count(std::string_view buffer) {
        auto hashMap = parse_file(buffer);

        std::vector<std::pair<std::string*, std::size_t*>*> vector;

        std::size_t words_count{};
        auto iter = hashMap->iterator();
        while(iter->hasNext()) {
            auto curr = iter->nextNode();
            words_count += *curr->value;
            auto pair = new std::pair(curr->key, curr->value);
            vector.push_back(pair);
        }

        std::cout << "Words: " << words_count << std::endl;

        for (auto elem : vector) {
            std::cout << *elem->first <<" : " << *elem->second << std::endl;
        }
        delete hashMap;

        for (auto elem : vector) {
            delete elem;
        }
    }
}

void measure(const std::function<void()>& fn) {
    auto start = std::chrono::high_resolution_clock::now();
    fn();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "count [--nogc | --gc [--mc | --ms]] <filename>" << std::endl;
        return 1;
    }
    std::ifstream file(argv[2]);

    if (!file.is_open()) {
        std::cout << "cannot open file: " << argv[2] << std::endl;
        return 2;
    }

    std::string buffer((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

    file.close();
    std::cout << "File size: " << buffer.size() << " symbols" << std::endl;

    if (std::strcmp(argv[1], "--nogc") == 0) {
        measure([&]() {
            unmanaged::count(buffer);
        });
    } else if (std::strcmp(argv[1], "--gc") == 0) {
        gccpp::Enviroment::init(std::make_unique<gccpp::MarkAndCompactCollector>(1024 * 1024 * 32));
        measure([&]() {
            managed::count(buffer);
        });
    } else {
        std::cerr << "Unknown option: " << argv[1] << std::endl;
        return 1;
    }

    return 0;
}