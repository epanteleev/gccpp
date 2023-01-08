#pragma once

#include "ShadowStack.h"
#include "pointer/GcRoot.h"
#include "pointer/GcPtr.h"

namespace tgc {

    class Frame final {
    public:
        explicit Frame(details::ShadowStack &stack) : m_stack(stack) {
            saved_sp = m_stack.enter();
        }

        template<typename Type>
        GcRoot <Type> root(GcPtr <Type> type) noexcept {
            return GcRoot<Type>(m_stack.push(type));
        }

        ~Frame() {
            m_stack.leave(saved_sp);
        }

    private:
        std::size_t saved_sp;
        details::ShadowStack &m_stack;
    };
}
