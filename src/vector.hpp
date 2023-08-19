#pragma once
#include "../src/head.hpp"
using namespace std;

template <typename T>
class LockedVector
{
public:
    void push(const T &value)
    {
        unique_lock<std::mutex> lock(mutex_);
        vector_.push_back(value);
    }

    T pop()
    {
        unique_lock<std::mutex> lock(mutex_);
        if (vector_.empty())
        {
            throw std::out_of_range("Vector is empty");
        }
        T value = vector_.front();
        vector_.erase(vector_.begin());
        return value;
    }

    T &operator[](size_t index)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return vector_[index];
    }

    T front()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return vector_.front();
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return vector_.empty();
    }

    typename std::vector<T>::iterator begin()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return vector_.begin();
    }

    typename std::vector<T>::iterator end()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return vector_.end();
    }

private:
    std::vector<T> vector_;
    std::mutex mutex_;
};
