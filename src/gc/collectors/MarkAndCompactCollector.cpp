#include "gc/collectors/MarkAndCompactCollector.h"
#include "gc/operations/Mark.h"
#include "gc/operations/Compact.h"
#include "gc/operations/UpdateReference.h"
#include "gc/operations/Relocate.h"

namespace gccpp {
    void MarkAndCompactCollector::safepoint_at_poll() {
        details::Mark mark;
        mark.do_it(this);

        details::Compact compact;
        compact.do_it(this);

        details::UpdateReference updateReference;
        updateReference.do_it(this);

        details::Relocate relocate;
        relocate.do_it(this);
    }
}