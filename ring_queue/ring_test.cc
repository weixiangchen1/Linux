#include "ring_queue.hpp"
#include "Task.hpp"
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

using namespace ns_task;
using namespace ns_ring_queue;

void* consumer(void* args)
{
    RingQueue<Task>* rq = (RingQueue<Task>*)args;

    while(true)
    {
        Task t;
        rq->Pop(&t);
        t();
        // sleep(1);
        // int data;
        // rq->Pop(&data);
        // std::cout << "Thread:" << pthread_self() << "消费者消费数据：" << data << std::endl;
        // sleep(1);
    }
}

void* producter(void* args)
{
    RingQueue<Task>* rq = (RingQueue<Task>*)args;
    while(true)
    {
        int x = rand()%20 + 1;
        int y = rand()%10 + 1;
        char op = "+-*/%"[rand()%5];
        Task t(x, y, op);
        rq->Push(t);

        std::cout << "Producter Thread: " << pthread_self() << " Task: " << t.message() << std::endl; 

        // int data = rand()%30 + 1;
        // rq->Push(data);
        // std::cout << "Thread:" << pthread_self() << "生产者生产数据：" << data << std::endl;
    }
}

int main()
{
    srand((long long)time(nullptr));
    RingQueue<Task>* rq = new RingQueue<Task>();
    
    pthread_t c0,c1,c2,c3,p0,p1,p2;
    pthread_create(&c0, nullptr, consumer, (void*)rq);
    pthread_create(&c1, nullptr, consumer, (void*)rq);
    pthread_create(&c2, nullptr, consumer, (void*)rq);
    pthread_create(&c3, nullptr, consumer, (void*)rq);
    pthread_create(&p0, nullptr, producter, (void*)rq);
    pthread_create(&p1, nullptr, producter, (void*)rq);
    pthread_create(&p2, nullptr, producter, (void*)rq);

    pthread_join(c0, nullptr);
    pthread_join(c1, nullptr);
    pthread_join(c2, nullptr);
    pthread_join(c3, nullptr);
    pthread_join(p0, nullptr);
    pthread_join(p1, nullptr);
    pthread_join(p2, nullptr);

    return 0;
}