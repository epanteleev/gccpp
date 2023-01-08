#include "Gc.h"

#include "gc/operations/Mark.h"
#include "gc/operations/Sweep.h"

void tgc::Gc::safepoint() {
    Mark mark;
    mark.do_it(this);

    Sweep sweep;
    sweep.do_it(this);
}