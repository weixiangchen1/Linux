#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t id = fork();
    if(id == 0){
        //child
        int cnt = 3;
        while(cnt){
            printf("I am a child process\n");
            cnt--;
            sleep(1);
        }
        exit(1);
    }
    //parent
    sleep(5);
    int status = 0;
    pid_t res = wait(&status);
    if(res > 0){
        //父进程等待成功
        printf("parent wait success, status exit code: %d, status exit single: %d, core dump: %d\n", (status>>8)&0xFF, status&0x7F, status&0x80);
    }
    else{
        //父进程等待失败
        printf("parent wait failed\n");
    }
    sleep(3);
    
    return 0;
}
