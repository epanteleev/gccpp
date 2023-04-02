#include "MarkAndSweepCollector.h"

#include "gc/operations/Mark.h"
#include "gc/operations/Sweep.h"

namespace gccpp {

    void MarkAndSweepCollector::collect() {
        details::Mark mark(worklist);
        mark.do_it(this);

        details::Sweep sweep;
        sweep.do_it(this);
    }
}
