#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>

class WaitBarrier final {
public:
    void wait(const std::function<bool()>& fn) {
        int lock_err = pthread_mutex_lock(&ready_mutex);
        assert_linux_error(lock_err);
        while (!fn()) {
            int may_err = pthread_cond_wait(&ready_cond, &ready_mutex);
            assert_linux_error(may_err);
        }
        int unlock_err = pthread_mutex_unlock(&ready_mutex);
        assert_linux_error(unlock_err);
    }

    inline void notify() noexcept {
        int lock_err = pthread_mutex_lock(&ready_mutex);
        assert_linux_error(lock_err);
        int err = pthread_cond_broadcast(&ready_cond);
        assert_linux_error(err);
        int unlock_err = pthread_mutex_unlock(&ready_mutex);
        assert_linux_error(unlock_err);
    }

private:
    pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;
};
