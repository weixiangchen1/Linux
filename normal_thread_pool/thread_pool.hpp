#pragma once

#include <iostream>
#include <queue>
#include <string>
#include <pthread.h>

// 基于工作队列的线程池
namespace ns_thread_pool
{
    const int g_num = 10;
    template <class T>
    class ThreadPool
    {
    private:
        std::queue<T> _task_queue; // 工作队列
        int _num;                  // 工作队列的容量上限
        pthread_mutex_t _mtx;      // 保护临界资源工作队列的锁  
        pthread_cond_t _cond;

    public:
        void Lock()
        {
            pthread_mutex_lock(&_mtx);
        }

        void Unlock()
        {
            pthread_mutex_unlock(&_mtx);
        }

        void Wakeup()
        {
            pthread_cond_signal(&_cond);
        }

        void Wait()
        {
            pthread_cond_wait(&_cond, &_mtx);
        }

        bool IsEmpty()
        {
            return _task_queue.empty();
        }

    public:
        ThreadPool(int num = g_num) : _num(num)
        {
            pthread_mutex_init(&_mtx, nullptr);
            pthread_cond_init(&_cond, nullptr);
        }

        static void *Rountine(void *args)
        {
            pthread_detach(pthread_self());
            ThreadPool<T> *tp = (ThreadPool<T> *)args;
            while (true)
            {
                tp->Lock();
                while (tp->IsEmpty())
                {
                    tp->Wait();
                }
                T t;
                tp->PopTask(&t);
                tp->Unlock();

                t();
            }
        }

        void ThreadInit()
        {
            pthread_t tid;
            for (int i = 0; i < _num; i++)
            {
                pthread_create(&tid, nullptr, Rountine, (void *)this);
            }
        }

        void PushTask(const T &in)
        {
            Lock();
            _task_queue.push(in);
            Unlock();
            Wakeup();
        }

        void PopTask(T *out)
        {
            *out = _task_queue.front();
            _task_queue.pop();
        }

        ~ThreadPool()
        {
            pthread_mutex_destroy(&_mtx);
            pthread_cond_destroy(&_cond);
        }
    };
}