#include <stdio.h>
#include <unistd.h>

// 父子进程间通信的命名管道
int main()
{
    int pipe_fd[2] = {0};
    if(pipe(pipe_fd) < 0)
    {
        perror("pipe");
        return 1;
    }
    printf("%d %d\n", pipe_fd[0], pipe_fd[1]);

    pid_t id = fork();
    if(id < 0)
    {
        perror("fork");
        return 1;
    }
    else if(id == 0)
    {
        // child process -- read
        close(pipe_fd[1]); // 关闭写端
        char buffer[1024] = {0};
        while(1)
        {
            ssize_t s = read(pipe_fd[0], buffer, sizeof(buffer)-1);
            if(s > 0)
            {
                buffer[s] = 0;
                printf("%s\n", buffer);
            }
        }
    }
    else
    {
        // parent process -- write
        close(pipe_fd[0]); // 关闭读端
        char msg[64] = "I am parent";
        while(1)
        {
            write(pipe_fd[1], msg, sizeof(msg));
            sleep(1);
        }
    }


    return 0;
}