#include "thread_pool.hpp"
#include "Task.hpp"
#include <ctime>
#include <cstdlib>
#include <unistd.h>

using namespace ns_task;
using namespace ns_thread_pool;

int main()
{
    srand((long long)time(nullptr));
    ThreadPool<Task>* tp = new ThreadPool<Task>();
    tp->ThreadInit();
    while(true)
    {
        Task t(rand()%20+1, rand()%10+1, "+-*/%"[rand()%5]);
        tp->PushTask(t);
    }

    return 0;
}