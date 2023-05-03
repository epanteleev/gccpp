#pragma once

namespace gccpp {
    class ThreadEnv;
    class Environment;
    class MarkWord;

    template<typename T>
    class Handle;

    template<typename T>
    class Oop;

    namespace details {
        class ShadowStack;
        class Mark;
        class Sweep;
        class MarkIncrementally;
        class Reallocate;
        class Relocate;
        class UpdateReference; //Todo rename to 'UpdateReferences'
        class MarkIncrementally;
    }
}
