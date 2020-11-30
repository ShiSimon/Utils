#ifndef _UTILS_THREAD_SAFE_QUEUE_H_
#define _UTILS_THREAD_SAFE_QUEUE_H_
#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <constructor_magic.h>
template<typename T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue<T> (){};
    ThreadSafeQueue<T> (ThreadSafeQueue const& other)
    {
        std::lock_guard<std::mutex> lock(other._mutex);
        _queue = other._queue;
    }
    virtual ~ThreadSafeQueue<T>(){};

public:
    size_t getSize() const
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _queue.size();
    }
    bool  bEmpty() const
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _queue.empty();
    }
    void    push(T value)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.push(value);
        lock.unlock();
        _cv.notify_one(); 
    }
    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while(_queue.empty())
        {
            _cv.wait(lock);
        }
        value = _queue.front();
        _queue.pop();
    }
    bool  try_pop(T& value)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if(_queue.empty())
        {
            return false;
        }else
        {
            value = _queue.front();
            _queue.pop();
        }
        return true;
    }
private:
    std::queue<T> _queue;
    mutable std::mutex _mutex;
    std::condition_variable _cv;
};

#endif