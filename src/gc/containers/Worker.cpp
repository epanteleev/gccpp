#include "Worker.h"
#include "gc/containers/Environment.h"
#include "Memory.h"

namespace gccpp::details {

    void Worker::run(details::Worker& self) noexcept {
        while (!self.ctx.state.is_terminate()) {
            if (self.wait_job()) {
                return;
            }
            Timer timer;
            timer.start();

            std::lock_guard _lock(self.ctx.env_lock);
            self.ctx.thread_lock.stw();

            assert(self.ctx.self_suspend == true);
            self.ctx.gc->collect();

            self.ctx.self_suspend = false;
            mem::write_barrier();
            self.ctx.state.mutators_continue();
            self.ctx.thread_lock.run_world();

            timer.stop();
            self.ctx.recorder.append(timer);
        }
    }

    void Worker::start() {
        ctx.state.start();
        worker = std::thread(Worker::run, std::ref(*this));
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
        return false;
    }
}
