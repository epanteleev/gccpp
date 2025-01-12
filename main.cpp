
#include "gc/containers/Environment.h"
#include "gc/collectors/MarkAndSweepCollector.h"
#include "pointer/Handle.h"

int main(int argc, char **argv) {
    auto& env = gccpp::Environment::init(std::make_unique<gccpp::MarkAndSweepCollector>(4 * 1024 * 1024));
    gccpp::ThreadEnv _th(env);
    gccpp::HandleMark hm;

    return 0;
}
