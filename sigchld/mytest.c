#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

void GetChildSignal(int signo){
    //waitpid(-1, NULL, 0);
    printf("get a signal: %d, pid: %d\n", signo, getpid());
}

int main()
{
    //signal(SIGCHLD, GetChildSignal);
    signal(SIGCHLD, SIG_IGN);
    pid_t id = fork();
    if(id == 0){
        int cnt = 5;
        while(cnt--){
            printf("I am a child process, pid: %d\n", getpid());
            sleep(1);
        }
        exit(0);
    }

    while(1);

    return 0;
}
