#pragma once

#include <queue>

#include <Poco/Mutex.h>
#include <Poco/Condition.h>
#include <Poco/ScopedLock.h>

template<class T>
class SynchronizedQueue 
{
public:
    typedef Poco::ScopedLock<Poco::Mutex> ScopedLock;
    ~SynchronizedQueue() {}

    void Put(T message) 
    {
        mu_.lock();
        queue_.push(message);
        mu_.unlock();
        cv_.signal();
    }
    T Get() 
    {
        ScopedLock l(mu_);
        if(queue_.empty())
        {
            cv_.wait(mu_);
        }
        T head = queue_.front();
        queue_.pop();
        return head;
    }
    int GetSize() const
    {
         ScopedLock l(mu_);
         return queue_.size();
    }
    std::queue<T> GetQueue() const
    {
        ScopedLock l(mu_);
        return queue_;
    }
private:
    std::queue<T> queue_;
    Poco::Mutex mu_;
    Poco::Condition cv_;
};