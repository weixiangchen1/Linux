#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// pipefd[2]：输出型参数
// 创建成功返回0，错误返回-1
// int pipe(int pipefd[2]);

// 1、读端暂停读数据或者读端读数据比较慢，写端等待读端(write调用阻塞)。
// 2、写端暂停写数据或者写端写数据比较慢，读端等待写端(read调用阻塞)。
// 3、读端关闭，写端收到操作系统发送的SIGPIPE信号终止进程。
// 4、写端关闭，读端读完匿名管道内部数据后，读到0，表示读到文件结尾。

int main()
{   
    int pipe_fd[2] = {0};
    if(pipe(pipe_fd) != 0){
        perror("create pipe");
        return 1;
    }

    // 父进程读取，子进程写入
    if(fork() == 0){
        // 子进程
        close(pipe_fd[0]); // 关闭读端

        const char* msg = "hello world ";
        while(1){
            write(pipe_fd[1], msg, strlen(msg));
            sleep(3);
        }
        exit(0);
    }

    // 父进程
    close(pipe_fd[1]); // 关闭写端
    while(1){
        char buffer[64] = {0};
        ssize_t s = read(pipe_fd[0], buffer, sizeof(buffer)-1);
        if(s == 0){
            printf("child process quit...\n");
            break;
        }
        else if(s > 0){
            buffer[s] = 0;
            printf("child process: %s\n", buffer);
            break;
        }
        else{
            printf("read error\n");
            break;
        }
    }
    close(pipe_fd[0]); // 关闭读端
    // 进程等待获取退出信号
    int status = 0;
    waitpid(-1, &status, 0);
    printf("exit signal: %d\n", status&0x7F);

    return 0;
}