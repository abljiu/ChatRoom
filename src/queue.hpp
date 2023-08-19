#pragma once
#include "../src/head.hpp"
using namespace std;

template <typename T>
class LockedQueue
{
public:
    void push(const T &value)
    {
        unique_lock<std::mutex> lock(mutex_);
        queue_.push(value);
        lock.unlock();
        condition_.notify_one();
    }

    T pop()
    {
        unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this]
                        { return !queue_.empty(); });
        T value = queue_.front();
        queue_.pop();
        return value;
    }
    T &operator[](size_t index)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_[index];
    }
    T front()
    {
        return queue_.front();
    }
    bool empty()
    {
        return queue_.empty();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable condition_;
};
