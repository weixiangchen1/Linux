#pragma once

#include <iostream>
#include <queue>
#include <string>
#include <pthread.h>

// 懒汉模式单例模式多线程池
namespace ns_thread_pool
{
    const int g_num = 10;

    template <class T>
    class ThreadPool
    {
    private:
        std::queue<T> _task_queue;   // 工作队列 -- 临界资源
        int _num;                    // 工作队列的容量
        pthread_mutex_t _mtx;        // 保护工作队列临界资源的锁
        pthread_cond_t _cond;        // 队列为空，在此环境变量等待

        static ThreadPool<T>* inst;  // 单例模式的静态对象指针

    private:
        // 单例模式构造函数必须定义以及必须为私有private
        ThreadPool(int num = g_num) : _num(num)
        {
            // 互斥锁和环境变量初始化
            pthread_mutex_init(&_mtx, nullptr);
            pthread_cond_init(&_cond, nullptr);
        }

        ThreadPool(const ThreadPool<T> &tp) = delete;
        ThreadPool<T> &operator=(ThreadPool<T> &tp) = delete;

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
        static ThreadPool<T> *GetInstance()
        {
            // 静态互斥锁初始化
            static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
            // 双条件判断可以减少锁的争用，提高效率
            if(inst == nullptr)
            {
                pthread_mutex_lock(&lock);
                if(inst == nullptr)
                {
                    inst = new ThreadPool<T>();
                    inst->ThreadInit();
                }
                pthread_mutex_unlock(&lock);
            }

            return inst;
        }

        static void *Rountine(void *args)
        {
            pthread_detach(pthread_self());  // 线程分离，主线程无需关注该线程
            ThreadPool<T> *tp = (ThreadPool<T> *)args;
            while (true)
            {
                tp->Lock();
                while (tp->IsEmpty())
                {
                    // 工作队列为空，线程需要等待
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
            // 创建_num个线程的多线程池
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

    template<class T>
    ThreadPool<T>* ThreadPool<T>::inst = nullptr;
}