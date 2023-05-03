#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>

class WaitBarrier final {
public:
    void wait(const std::function<bool()>& fn) {
        pthread_mutex_lock(&ready_mutex);
        while (!fn()) {
            pthread_cond_wait(&ready_cond, &ready_mutex);
        }
        pthread_mutex_unlock(&ready_mutex);
    }

    inline void notify() noexcept {
        pthread_mutex_lock(&ready_mutex);
        pthread_cond_broadcast(&ready_cond);

        pthread_mutex_unlock(&ready_mutex);
    }

private:
    pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;
};
