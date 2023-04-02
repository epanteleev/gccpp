#include <iostream>
#include <thread>
#include "gc/containers/Enviroment.h"
#include "pointer/Oop.inline.h"
#include "gc/GarbageCollected.h"
#include "gc/collectors/MarkAndSweepCollector.h"
#include "gc/operations/GCOperation.h"

int main(int argc, char **argv) {
    auto& env = gccpp::Enviroment::init(std::make_unique<gccpp::MarkAndSweepCollector>());
    gccpp::ThreadEnv _th(env);
    gccpp::HandleMark hm(_th);

    return 0;
}
