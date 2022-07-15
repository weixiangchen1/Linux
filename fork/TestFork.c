#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int ret = fork();
    if(ret < 0)
    {
        perror("fork");
        return 1;
    }
    else if(ret == 0)
    {
        //子进程
        printf("I am child, pid : %d, ppid : %d\n", getpid(), getppid());
    }
    else
    {
        //父进程
        printf("I am parent, pid : %d, ppid : %d\n", getpid(), getppid());
    }
    sleep(1);
    
    return 0;
}

