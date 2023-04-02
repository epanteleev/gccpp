#include "Worker.h"
#include "gc/containers/Enviroment.h"
#include "Memory.h"

namespace gccpp::details {

    void Worker::run(details::Worker& self) noexcept {
        self.ctx.state.wait_start();

        if (self.is_terminate()) {
            return;
        }

        while (!self.is_terminate()) {
            if (self.wait_job()) {
                return;
            }

            self.ctx.global_lock.stw();

            self.ctx.gc->collect();

            self.ctx.self_suspend = false;
            details::mem::write_barrier();
            self.ctx.global_lock.run_world();
            self.ctx.state.mutators_continue();

        }
    }

    bool Worker::is_terminate() {
        return ctx.state.is_terminate();
    }

    void Worker::collect() {
        ctx.state.collect();
    }

    void Worker::start() {
        ctx.state.start();
    }

    void Worker::stop() {
        ctx.state.terminate();
        worker.join();
    }

    bool Worker::wait_job() {
        ctx.state.wait_collect_command();

        if (ctx.state.is_terminate()) {
            return true;
        }
        ctx.self_suspend = true;
        details::mem::write_barrier();
        return false;
    }
}
