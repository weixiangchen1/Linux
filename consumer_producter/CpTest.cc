#include "BlockQueue.hpp"
#include "Task.hpp"
#include <time.h>
#include <cstdlib>
#include <unistd.h>

using namespace ns_blockqueue;
using namespace ns_task;
void *consumer(void *args)
{
    BlockQueue<Task> *bq = (BlockQueue<Task> *)args;

    while(true)
    {
        Task t;
        bq->Pop(&t);
        t();

        // int data = 0;
        // bq->Pop(&data);
        // std::cout << "消费者消费数据：" << data << std::endl;
    }

}

void *producter(void *args)
{
    BlockQueue<Task> *bq = (BlockQueue<Task> *)args;
    std::string ops = "+-*/%";
    while(true)
    {
        int x = rand()%10 + 1;
        int y = rand()%20 + 1;
        char op = ops[rand()%5];
        Task t(x, y, op);
        std::cout << "线程: " << pthread_self() <<  " 分发任务 " << x << op << y << "=?" << std::endl;
        bq->Push(t);
        sleep(1);

        //sleep(1);
        // int data = rand()%20 + 1;
        // bq->Push(data);   
        // std::cout << "生产者成产数据：" << data << std::endl;

    }
}

int main()
{
    srand((long long)time(nullptr));
    BlockQueue<Task> *bq = new BlockQueue<Task>();

    pthread_t c, p;
    pthread_t c1, c2, c3;
    pthread_t p1, p2, p3;
    pthread_create(&c, nullptr, consumer, (void*)bq);
    // pthread_create(&c1, nullptr, consumer, (void*)bq);
    // pthread_create(&c2, nullptr, consumer, (void*)bq);
    // pthread_create(&c3, nullptr, consumer, (void*)bq);

    pthread_create(&p, nullptr, producter, (void*)bq);
    // pthread_create(&p1, nullptr, producter, (void*)bq);
    // pthread_create(&p2, nullptr, producter, (void*)bq);
    // pthread_create(&p3, nullptr, producter, (void*)bq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);

    return 0;
}
