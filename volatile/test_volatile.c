#include <stdio.h>
#include <signal.h>

volatile int flag = 0; // 保持内存的可见性

void handler(int signo){
    flag = 1;
    printf("get a signo: %d, flag 0 to 1\n", signo);
}

int main()
{
    signal(2, handler);
    while(!flag);
    printf("process normal quit..\n");

    return 0;
}
