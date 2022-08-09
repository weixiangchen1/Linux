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
    while(true)
    {
        Task t(rand()%20+1, rand()%10+1, "+-*/%"[rand()%5]);
        ThreadPool<Task>::GetInstance()->PushTask(t);
        sleep(1);
    }

    return 0;
}