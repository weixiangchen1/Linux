#include "ring_queue.hpp"
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

using namespace ns_ring_queue;

void* consumer(void* args)
{
    RingQueue<int>* rq = (RingQueue<int>*)args;

    while(true)
    {
        int data;
        rq->Pop(&data);
        std::cout << "消费者消费数据：" << data << std::endl;
        sleep(1);
    }
}

void* producter(void* args)
{
    RingQueue<int>* rq = (RingQueue<int>*)args;
    while(true)
    {
        int data = rand()%30 + 1;
        rq->Push(data);
        std::cout << "生产者生产数据：" << data << std::endl;
    }
}

int main()
{
    srand((long long)time(nullptr));
    RingQueue<int>* rq = new RingQueue<int>();
    
    pthread_t c,p;
    pthread_create(&c, nullptr, consumer, (void*)rq);
    pthread_create(&p, nullptr, producter, (void*)rq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);

    return 0;
}