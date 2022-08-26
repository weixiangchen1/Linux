#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* thread_run(void *args)
{
    pthread_detach(pthread_self()); // 新线程分离
    printf("%s running...\n", (const char*)args);
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, thread_run, (void*)"new thread");
    sleep(1);

    if(pthread_join(tid, NULL) == 0){
        printf("pthread join success\n");
    }
    else{
        printf("pthread wait fail\n");
    }

    return 0;
}


// pthread_t g_tid;

// void* thread_run(void* args)
// {
//     while(1)
//     {
//         printf("5s after cancel main thread, %s running...\n", (const char*)args);
//         sleep(5);
//         pthread_cancel(g_tid);
//     }
// }

// int main()
// {
//     g_tid = pthread_self();
//     pthread_t tid;
//     pthread_create(&tid, NULL, thread_run, (void*)"new thread");

//     sleep(20);
//     pthread_join(tid, NULL);

//     return 0;
// }


// void* pthread_run(void* args)
// {
//     while(1)
//     {
//         printf("%s runing...\n", (const char*)args);
//         sleep(1);
//     }
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, NULL, pthread_run, (void*)"new thread");
 
//     printf("3s cancel new thread...\n");
//     sleep(3);
//     pthread_cancel(tid);  // 主线程取消新线程

//     void *status = NULL;
//     pthread_join(tid, &status);
//     printf("exit code: %d\n", (int)status);

//     return 0;
// }

// void* pthread_run(void* args)
// {
//     printf("%s running...\n", (const char*)args);

//     pthread_exit((void*)123); 
//     // return (void*)123; // 返回退出码123
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, NULL, pthread_run, (void*)"new thread");

//     void* status = NULL; // 获取退出码
//     pthread_join(tid, &status);

//     printf("exit code: %d\n", (int)status);

//     return 0;
// }


// void* pthread_run(void* args)
// {
//     int num = *(int*)args;
//     while(1)
//     {
//         printf("I am %d thread, pid: %d, id: %lu\n", num, getpid(), pthread_self());
//         sleep(1);
//     }
// }

// int main()
// {
//     pthread_t pid[5];
//     for(int i=0; i<5; i++)
//     {
//         pthread_create(pid+i, NULL, pthread_run, (void*)&i); //创建新线程执行pthread_run
//         sleep(1);
//     }
//     for(int i=0; i<5; i++)
//     {
//         printf("thread %d id: %lu\n", i, pid[i]);
//     }

//     return 0;
// }


