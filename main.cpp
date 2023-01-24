#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec;

    for(auto i = vec.begin(); i != vec.end(); ++i) {
        std::cout << "Hello, World!" << std::endl;
    }

    return 0;
}
