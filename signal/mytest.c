#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int count = 0;

void HandlerAlarm(int signo){
    printf("count = %d\n", count);
}

int main()
{
    signal(SIGALRM, HandlerAlarm);
    alarm(1);
    while(1){
        count++;
       // printf("count = %d\n", count);
    }


    return 0;
}

//void handler(int signo){
//    printf("signo: %d\n", signo);
//}
//
//int main()
//{
//    int sig = 1;
//    for(; sig <= 31; sig++){
//        signal(sig, handler);
//    }
//    int res = alarm(20);
//
//    while(1)
//    {
//        printf("hello\n");
//        sleep(3);
//        int ret = alarm(0);
//        printf("res: %d. ret: %d\n", res, ret);
//
//    }
//
//
//
//    return 0;
//}
//

//void handler(int signo){
//    printf("get a signal: signo :%d, pid: %d\n", signo, getpid());
//}
//
//int main()
//{
//    int sig = 1;
//    for(; sig <= 31; sig++){
//        signal(sig, handler);
//    }
//
//    while(1){
//        printf("hello world\n");
//      
//        abort();
//        //raise(9);
//    }
//
//    return 0;
//}
//

//static void Usage(const char* proc){
//    printf("Usage:\n\t %s signo who\n", proc);
//}
//
//int main(int argc, char *argv[]){
//    if(argc != 3){
//        Usage(argv[0]);
//        return 1;
//    }
//
//    int signo = atoi(argv[1]);
//    int who = atoi(argv[2]);
//    kill(who ,signo);
//
//    printf("signo: %d, who: %d\n", signo, who);
//
//    return 0;
//}
//
//int main()
//{
//    if(fork() == 0){
//        while(1){
//            printf("hello world\n");
//            int a = 10;
//            a /= 0;
//        }
//    }
//    
//    int status = 0;
//    waitpid(-1, &status, 0);
//    printf("exit code: %d, exit signal: %d, core dump: %d\n", (status>>8)&0xFF, status&0x7F, (status>>7)&1);
//
//    return 0;
//}


//int main()
//{
//    int sig = 1;
//    for(; sig <= 31; sig++){
//        signal(sig, handler);
//    }
//    while(1){
//        int *p = NULL;
//        p = (int*)100;
//        *p = 100; // 野指针->11号信号 SIGSEGV
//
//        //int a = 10;
//        //a /= 0;  // 浮点错误->8号信号 SIGFPE
//        printf("hello world\n");
//        sleep(1);
//    }
//
//
//    return 0;
//}


//int main()
//{
//    while(1){
//        signal(2, handler);
//        printf("hello world, pid: %d\n", getpid());
//        sleep(1);
//    }
//
//
//    return 0;
//}
