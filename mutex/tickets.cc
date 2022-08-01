#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <stdlib.h>
#include <string>
#include <pthread.h>

class Ticket
{
private:
    int tickets;
    pthread_mutex_t mtx; 

public:
    Ticket()
        :tickets(100000)
    {
        pthread_mutex_init(&mtx, nullptr); //初始化锁
    }

    bool GetTicket()
    {
        bool res = true;

        pthread_mutex_lock(&mtx); //申请锁,临界区加锁,访问临界资源
        if(tickets > 0)
        {
            usleep(100);
            std::cout <<  pthread_self() << " thread buy " << "No." << tickets << " ticket" << std::endl;
            tickets--; 
            printf("");
        }
        else
        {
            std::cout << "have no ticket" << std::endl;
            res = false;
        }
        pthread_mutex_unlock(&mtx); // 解锁

        return res;
    }

    ~Ticket()
    {
        pthread_mutex_destroy(&mtx);
    }
};

void *ThreadRun(void *args)
{
    Ticket *t = (Ticket*)args;

    while(true)
    {
        if(!t->GetTicket())
             break;
        //printf("%lu\n", pthread_self());
    }

}

int main()
{
    Ticket *t = new Ticket();
    pthread_t tid[5];
    for(int i = 0; i < 5; i++)
    {
        //创建5个线程,每个线程执行ThreadRun函数
        pthread_create(tid+i, nullptr, ThreadRun, (void*)t);
    }

    for(int i = 0; i < 5; i++)
    {
        //线程等待
        pthread_join(tid[i], nullptr);
    }


    return 0;
}
