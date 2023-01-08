#include "MarkAndSweepCollector.h"

#include "gc/operations/Mark.h"
#include "gc/operations/Sweep.h"

namespace gccpp {
    void MarkAndSweepCollector::safepoint_at_poll() {
        details::Mark mark;
        mark.do_it(this);

        details::Sweep sweep;
        sweep.do_it(this);
    }
}
