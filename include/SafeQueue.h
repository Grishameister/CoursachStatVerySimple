#ifndef HIGHLOAD_SERVER_SAFEQUEUE_H
#define HIGHLOAD_SERVER_SAFEQUEUE_H

#include <queue>
#include <condition_variable>

template<typename T>
class SafeQueue {
private:
    std::queue<T> q;
    mutable std::mutex mu;
    std::condition_variable cv;
public:
    bool Push(T value) {
        std::lock_guard<std::mutex> guard(mu);
        q.push(std::move(value));
        cv.notify_one();
        return true;
    }

    bool WaitPop(T& value) {
        std::unique_lock<std::mutex> guard(mu);
        cv.wait(guard, [this]() {
            return !q.empty();
        });
        value = std::move(q.front());
        q.pop();
        return false;
    }

        bool Empty() const {
        std::lock_guard<std::mutex> guard(mu);
        return q.empty();
    }
    size_t Size() const {
        std::lock_guard<std::mutex> guard(mu);
        return q.size();
    }
};

#endif //HIGHLOAD_SERVER_SAFEQUEUE_H
