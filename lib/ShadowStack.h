#pragma once
#include <vector>
#include <stack>

namespace tgc::details {
    class ShadowStack final {
    public:
        using size_type = std::vector<PtrBase>::size_type;

    public:
        ShadowStack() = default;
        ~ShadowStack() = default;

    public:
        void enter() {
            frame.push(stack.size());
        }

        void push(PtrBase ptr) {
            stack.push_back(ptr);
        }

        void leave() {
            auto fd = frame.top();
            stack.erase(stack.begin(), stack.begin() + fd);
            frame.pop();
        }

    private:
        std::vector<PtrBase> stack;
        std::stack<size_type> frame;
    };
}
