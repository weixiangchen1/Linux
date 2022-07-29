#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>


void handler(int signo){
    while(1){
        printf("get a signo: %d\n", signo);
        //exit(1);
        sleep(1);
    }
}
int main()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    //act.sa_handler = handler;
    //act.sa_handler = SIG_IGN;
    act.sa_handler = SIG_DFL;

    sigemptyset(&act.sa_mask);
    //sigaddset(&act.sa_mask, 3);

    sigaction(2, &act, NULL);
    while(1){
        printf("hello world\n");
        sleep(1);
    }


    return 0;
}


//void show_pending(sigset_t *pending){
//    printf("process pending: ");
//    int sig = 1;
//    for(; sig <= 31; sig++){
//        if(sigismember(pending, sig)){
//            printf("1");
//        }
//        else{
//            printf("0");
//        }
//    }
//    printf("\n");
//}
//
//void handler(int signo){
//    printf("%d信号可被递达\n", signo);
//}
//
//int main()
//{
//    sigset_t iset, oset;
//    sigemptyset(&iset); // 将信号集全设置为0
//    sigemptyset(&oset);
//
//    signal(2, handler); // 将2号信号的处理方式设为自定义
//    sigaddset(&iset, 2); // 设置iset中的二号信号
//    sigprocmask(SIG_SETMASK, &iset, &oset); // 屏蔽进程的二号信号
//    
//    int count = 0;
//    sigset_t pending;
//    while(1){
//        sigemptyset(&pending);
//
//        sigpending(&pending); // 获取进程的信号位图
//
//        show_pending(&pending); // 打印进程的信号位图
//        sleep(1);
//        count++;
//        if(count == 8){
//            sigprocmask(SIG_SETMASK, &oset, NULL);
//        }
//    }
//
//    return 0;
//}
