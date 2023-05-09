#pragma once

namespace gccpp {
    class ThreadEnv;
    class Environment;
    class MarkWord;
    class BasicCollector;
    class MarkAndSweepCollector;

    template<typename T>
    class Handle;

    template<typename T>
    class Oop;

    namespace details {
        class ObjectPointer;
        class ShadowStack;
        class Mark;
        class Sweep;
        class Reallocate;
        class Relocate;
        class UpdateReferences;
    }
}
