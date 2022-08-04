#pragma once
#include <iostream>
#include <vector>
#include <semaphore.h>

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

    public:
        RingQueue(int cap = cap_default) : _ring_queue(cap), _cap(cap)
        {
            sem_init(&_sem_blank, 0, _cap);
            sem_init(&_sem_data, 0, 0);
            c_step = p_step = 0;
        }

        void Push(const T &in)
        {
            sem_wait(&_sem_blank);

            _ring_queue[p_step] = in;
            sem_post(&_sem_data);

            p_step++;
            p_step %= _cap;
        }

        void Pop(T* out)
        {
            sem_wait(&_sem_data);
            
            *out = _ring_queue[c_step];
            sem_post(&_sem_blank);

            c_step++;
            c_step %= 10;
        }

        ~RingQueue()
        {
            sem_destroy(&_sem_blank);
            sem_destroy(&_sem_data);
        }
    };
}