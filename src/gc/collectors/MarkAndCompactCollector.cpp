#include "gc/collectors/MarkAndCompactCollector.h"
#include "gc/operations/Mark.h"
#include "gc/operations/Reallocate.h"
#include "gc/operations/UpdateReferences.h"
#include "gc/operations/Relocate.h"

namespace gccpp {

    void MarkAndCompactCollector::collect() {
        details::Mark mark(worklist);
        mark.do_it(this);

        details::Reallocate compact(worklist);
        compact.do_it(this);

        details::UpdateReferences updateReference(worklist);
        updateReference.do_it(this);

        details::Relocate relocate;
        relocate.do_it(this);
    }
}