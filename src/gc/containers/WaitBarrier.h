#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>

class WaitBarrier final {
public:
    void wait(const std::function<bool()>& fn) {
        std::unique_lock<std::mutex> _lock(mutex);

        cv.wait(_lock, fn);
    }

    inline void notify() noexcept {
        cv.notify_all();
    }

private:
    std::condition_variable cv;
    std::mutex mutex;
};
