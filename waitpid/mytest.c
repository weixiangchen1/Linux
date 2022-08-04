#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{   
    pid_t id = fork();
    if(id == 0)
    {
        int cnt = 10;
        while(cnt--)
        {
            printf("child process running, pid: %d, ppid: %d\n", getpid(), getppid());
            sleep(1);
        }
        exit(2);
    }

    // 父进程非阻塞等待
    int status = 0;
    while(1)
    {
        pid_t ret = waitpid(id, &status, WNOHANG);
        if(ret == 0)
        {
            // 子进程未退出，父进程waitpid成功，父进程重复等待子进程退出
            printf("parent do himself\n");
            sleep(1);
        }
        else if(ret > 0)
        {
            // 子进程退出，父进程waitpid成功
            if(WIFEXITED(status))
            {
                printf("exit code: %d\n", WEXITSTATUS(status));
                break;
            }
            else
            {
                printf("get a signal\n");
                break;
            }
        }
        else
        {
            // 父进程等待失败
            printf("parent wait failed\n");
        }
    }



    // 父进程阻塞等待
    // // pid_ t waitpid(pid_t pid, int *status, int options);
    // int status = 0;
    // //pid_t ret = waitpid(-1, &status, 0); // pid=-1相当于wait
    // pid_t ret = waitpid(id, &status, 0); // 等待指定id的进程，options为0为阻塞等待
    // if(ret > 0)
    // {
    //     if(WIFEXITED(status))
    //     {
    //         printf("exit code: %d\n", WEXITSTATUS(status));
    //     }
    //     else
    //     {
    //         printf("get a signal\n");
    //     }
    // }
    // else
    // {
    //     printf("parent wait failed\n");
    // }


    return 0;
}