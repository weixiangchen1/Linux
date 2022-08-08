#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>

void sigcb(int signo)
{
    printf("get a signal: %d\n", signo);
}

int main()
{
    int i = 0;
    for( ; i <= 31; i++)
    {
        signal(i, sigcb);
    }

    while(1)
    {
        sleep(1);
        printf("hello world\n");
    }


    return 0;
}



//void sigcb(int signo)
//{
//    printf("get a signal: %d\n", signo);
//}
//
//int main()
//{
//    struct sigaction act;
//    sigemptyset(&act.sa_mask);
//    act.sa_handler = sigcb;
//
//    int i = 0;
//    for(; i <= 31; i++)
//    {
//        sigaction(i, &act, NULL);
//    }
//    while(1);
//
//    return 0;
//}


// void handler(int signo)
// {
//     printf("get a signal: %d\n", signo);
// }

// int main()
// {
//     signal(2, handler);
//     signal(40, handler);

//     pid_t id = getpid();
//     sigset_t sig;
//     sigemptyset(&sig);
//     sigaddset(&sig, 2);
//     sigaddset(&sig, 40);

//     sigprocmask(SIG_SETMASK, &sig, NULL);

//     int cnt = 5;
//     while(cnt--)
//     {
//         sleep(1);
//         kill(id, 2);
//         kill(id, 40);
//     }

//     return 0;
// }
