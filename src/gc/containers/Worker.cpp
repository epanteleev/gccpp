#include "Worker.h"
#include "gc/containers/GlobalCtx.h"

namespace gccpp::details {

    void Worker::run(details::Worker * self) noexcept {

        while (!self->is_started()) {
            if (self->is_collect()) { //Todo
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        while (!self->is_terminate()) {
            while (!self->is_collect()) {
                if (self->is_terminate()) {
                    return;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(11));
            }

            while (!self->ctx->global_lock.try_stw()) {
                if (self->is_terminate()) {
                    return;
                }
            }
            self->periodic_task();
            self->ctx->self_suspend = false;
            self->ctx->global_lock.run_world();
            self->start(); //todo
        }
    }

    bool Worker::is_terminate() {
        return mode.load() == 2;
    }

    bool Worker::is_started() {
        return mode.load() == 1;
    }

    bool Worker::is_collect() {
        return mode.load() == 3;
    }
}