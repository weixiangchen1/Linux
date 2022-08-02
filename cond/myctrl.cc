#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>

#define NUM 3

pthread_mutex_t mtx;
pthread_cond_t cond;

void *Worker(void *args)
{
    int id = *(int*)args;
    delete (int*)args;

    while(true)
    {
        pthread_cond_wait(&cond, &mtx); // 在cond条件变量下等待
        std::cout << "NO. " << id << " working..." << std::endl;
    }
}

void *Master(void *args)
{
    while(true)
    {
        std::cout << "master begin..." << std::endl;
        pthread_cond_signal(&cond); // 唤醒等待
        //pthread_cond_broadcast(&cond);
        sleep(1);
    }
}

int main()
{
    pthread_mutex_init(&mtx, nullptr);  //初始化锁
    pthread_cond_init(&cond, nullptr);  //初始化条件变量
    pthread_t master;
    pthread_t worker[NUM];
    // 创建NUM个线程
    for(int i = 0; i < NUM; i++)
    {
        int *id = new int(i);
        pthread_create(worker+i, nullptr, Worker, (void*)id);
    }
    pthread_create(&master, nullptr, Master, (void*)"master");

    // 线程等待
    for(int i = 0; i < NUM; i++)
    {
        pthread_join(worker[i], nullptr);
    }
    pthread_join(master, nullptr);

    pthread_mutex_destroy(&mtx);  //销毁锁
    pthread_cond_destroy(&cond);  //销毁环境变量

    return 0;
}