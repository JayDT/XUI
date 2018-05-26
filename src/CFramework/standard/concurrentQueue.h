#pragma once

#include <queue>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace System
{
    namespace Concurrent
    {
        template <typename T>
        class ActivationQueue
        {
        public:

            T pop()
            {
                std::unique_lock<std::mutex> mlock(mutex_);
                while (queue_.empty())
                {
                    cond_.wait(mlock);
                }
                auto item = queue_.front();
                queue_.pop_front();
                return item;
            }

            bool pop(T& item)
            {
                std::unique_lock<std::mutex> mlock(mutex_);
                while (queue_.empty())
                {
                    cond_.wait(mlock);
                }
                item = queue_.front();
                queue_.pop_front();
                return item != nullptr;
            }

            void push(const T& item)
            {
                std::unique_lock<std::mutex> mlock(mutex_);
                queue_.push_back(item);
                mlock.unlock();
                cond_.notify_one();
            }

            void push(T&& item)
            {
                std::unique_lock<std::mutex> mlock(mutex_);
                queue_.push_back(std::move(item));
                mlock.unlock();
                cond_.notify_one();
            }

            void push_front(const T& item)
            {
                std::unique_lock<std::mutex> mlock(mutex_);
                queue_.push_front(item);
                mlock.unlock();
                cond_.notify_one();
            }

            void push_front(T&& item)
            {
                std::unique_lock<std::mutex> mlock(mutex_);
                queue_.push_front(std::move(item));
                mlock.unlock();
                cond_.notify_one();
            }
        private:
            std::deque<T> queue_;
            std::mutex mutex_;
            std::condition_variable cond_;
        };

        template <typename T>
        class Queue
        {
        public:

            bool pop(T& item)
            {
                mutex_.lock();
                if (queue_.empty())
                {
                    mutex_.unlock();
                    return false;
                }
                item = queue_.front();
                queue_.pop_front();
                mutex_.unlock();
                return true;
            }

            void push_front(const T& item)
            {
                mutex_.lock();
                queue_.push_front(item);
                mutex_.unlock();
            }

            void push_front(T&& item)
            {
                mutex_.lock();
                queue_.push_front(std::move(item));
                mutex_.unlock();
            }

            void push(const T& item)
            {
                mutex_.lock();
                queue_.push_back(item);
                mutex_.unlock();
            }

            void push(T&& item)
            {
                mutex_.lock();
                queue_.push_back(std::move(item));
                mutex_.unlock();
            }

            bool empty() const
            {
                return queue_.empty();
            }

        private:
            std::deque<T> queue_;
            std::mutex mutex_;
        };
    }
}