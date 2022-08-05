#pragma once
#include <iostream>
#include <vector>
#include <semaphore.h>

// 基于环形队列的多生产者多消费者模型
namespace ns_ring_queue
{
    const int cap_default = 10;
    template <class T>
    class RingQueue
    {
    private:
        std::vector<T> _ring_queue; // 环形队列
        int _cap;                   // 环形队列的容量
        sem_t _sem_blank;           // 空位置信号量
        sem_t _sem_data;            // 数据信号量
        int c_step;                 // 消费者下标
        int p_step;                 // 生产者下标

        pthread_mutex_t c_mtx;      // 保护消费者的临界资源
        pthread_mutex_t p_mtx;      // 保护生产者的临界资源

    public:
        RingQueue(int cap = cap_default) : _ring_queue(cap), _cap(cap)
        {
            sem_init(&_sem_blank, 0, _cap);
            sem_init(&_sem_data, 0, 0);
            c_step = p_step = 0;
            pthread_mutex_init(&c_mtx, nullptr);
            pthread_mutex_init(&p_mtx, nullptr);
        }

        void Push(const T &in)
        {
            sem_wait(&_sem_blank); //P(blank)
            pthread_mutex_lock(&p_mtx);
            _ring_queue[p_step] = in;

            p_step++; // 临界资源
            p_step %= _cap;

            pthread_mutex_unlock(&p_mtx);
            sem_post(&_sem_data); //V(data)
        }

        void Pop(T* out)
        {
            sem_wait(&_sem_data); //P(data)
            pthread_mutex_lock(&c_mtx);
            *out = _ring_queue[c_step];

            c_step++; // 临界资源
            c_step %= _cap;

            pthread_mutex_unlock(&c_mtx);
            sem_post(&_sem_blank); //V(blank)
        }

        ~RingQueue()
        {
            sem_destroy(&_sem_blank);
            sem_destroy(&_sem_data);
            pthread_mutex_destroy(&p_mtx);
            pthread_mutex_destroy(&c_mtx);
        }
    };
}