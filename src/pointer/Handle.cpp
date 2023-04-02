#include "pointer/Handle.h"
#include "gc/containers/Enviroment.h"

namespace gccpp {

    HandleMark::HandleMark():
        saved_sp(details::ThreadsStacks::stack_of_current_thread().enter()) {}

    HandleMark::~HandleMark() {
        details::ThreadsStacks::stack_of_current_thread().leave(saved_sp);
    }
}