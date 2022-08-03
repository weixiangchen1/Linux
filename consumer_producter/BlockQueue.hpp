#pragma once
#include <iostream>
#include <queue>
#include <pthread.h>

namespace ns_blockqueue
{
    const int default_capacity = 5;
    template <class T>
    class BlockQueue
    {
    private:
        std::queue<T> _bq;       // 阻塞队列
        int _capacity;           // 队列的容量上限
        pthread_mutex_t _mtx;    // 保护临界资源的互斥锁
        pthread_cond_t is_full;  // 队列满，消费者条件变量等待
        pthread_cond_t is_empty; // 队列空，生产者条件变量等待


        bool IsFull()
        {
            return _bq.size() == _capacity;
        }

        bool IsEmpty()
        {
            return _bq.size() == 0;
        }

        void LockQueue()
        {
            pthread_mutex_lock(&_mtx);
        }

        void UnlockQueue()
        {
            pthread_mutex_unlock(&_mtx);
        }

        void ProducterWait()
        {
            pthread_cond_wait(&is_empty, &_mtx);
        }

        void ConsumerWait()
        {
            pthread_cond_wait(&is_full, &_mtx);
        }

        void WakeupConsumer()
        {
            pthread_cond_signal(&is_full);
        }

        void WakeupProducter()
        {
            pthread_cond_signal(&is_empty);
        }

    public:
        BlockQueue(int capacity = default_capacity) : _capacity(capacity)
        {
            pthread_mutex_init(&_mtx, nullptr);
            pthread_cond_init(&is_full, nullptr);
            pthread_cond_init(&is_empty, nullptr);
        }

        void Push(const T &in)
        {
            LockQueue();
            while (IsFull())
            {
                ProducterWait();
            }

            _bq.push(in);
            //if (_bq.size() > _capacity / 2)
                WakeupConsumer();
            UnlockQueue();
        }

        void Pop(T *out)
        {
            LockQueue();
            while (IsEmpty())
            {
                ConsumerWait();
            }
            
            *out = _bq.front();
            _bq.pop();
            //if (_bq.size() < _capacity / 2)
                WakeupProducter();
            UnlockQueue();
        }

        ~BlockQueue()
        {
            pthread_mutex_destroy(&_mtx);
            pthread_cond_destroy(&is_full);
            pthread_cond_destroy(&is_empty);
        }
    };
}
