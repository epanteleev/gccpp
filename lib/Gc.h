#pragma once

#include "pointer/GcPtr.h"
#include "ShadowStack.h"
#include "pointer/GcRoot.h"

namespace tgc {

    class Frame final {
    public:
        explicit Frame(details::ShadowStack& stack):
                m_stack(stack) {}

        ~Frame() {
            m_stack.leave();
        }
    private:
        details::ShadowStack& m_stack;
    };

    class Gc final {
    public:
        Gc() = default;
        ~Gc() = default;

    public:
        template<typename T, typename... Args>
        inline GcRoot<T> gc_alloc(Args &&... args) {
            auto *ptr = static_cast<std::byte *>(std::malloc(sizeof(ObjMeta) + sizeof(T)));
            if (ptr == nullptr) {
                return GcRoot<T>();
            }
            auto *meta = new(ptr) ObjMeta();
            auto start = ptr + sizeof(ObjMeta);
            new(start) T(std::forward<Args>(args)...);

            GcRoot<T> root(meta);
            shadowStack.push(details::PtrBase(root.meta()));
            return root;
        }

        inline Frame enter() {
            shadowStack.enter();
            return Frame(shadowStack);
        }

    private:
        details::ShadowStack shadowStack;
    };
}