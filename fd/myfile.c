#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    printf("stdin-> %d\n", stdin->_fileno);
    printf("stdout-> %d\n", stdout->_fileno);
    printf("stderr-> %d\n", stderr->_fileno);

    // 输入重定向
    //close(0);
    //int fd = open("./log1.txt", O_RDONLY);
    //printf("fd: %d\n", fd);
    //char line[128];
    //while(fgets(line, sizeof(line)-1, stdin)){
    //    printf("%s\n", line);
    //}


    // 输出重定向
    //close(1);
    //int fd = open("./log1.txt", O_WRONLY);
    //printf("hello printf\n");
    //printf("hello printf\n");
    //printf("hello printf\n");
    //fprintf(stdout, "hello fprintf\n");
    //fprintf(stdout, "hello fprintf\n");
    //fprintf(stdout, "hello fprintf\n");
    // fflush(stdout);
   
    // close(fd);

    //int fd1 = open("./log1.txt", O_RDONLY);
    //if(fd1 < 0){
    //    perror("open");
    //    return 1;
    //}
    //char buffer[1024] = {0};
    //ssize_t s = read(fd1, buffer, sizeof(buffer)-1);
    //if(s > 0){
    //    buffer[s] = 0;
    //    printf("%s\n", buffer);
    //}
    //close(fd1);



    //int fd1 = open("./log1.txt", O_WRONLY | O_CREAT, 0644);
    //int fd2 = open("./log2.txt", O_WRONLY | O_CREAT, 0644);
    //int fd3 = open("./log3.txt", O_WRONLY | O_CREAT, 0644);

    // 0为标准输入，1为标准输出，2为标准错误
    //printf("fd1: %d\n", fd1); //3
    //printf("fd2: %d\n", fd2); //4
    //printf("fd3: %d\n", fd3); //5

    //const char* msg = "hello\n";
    //int cnt = 5;
    //while(cnt--){
    //    write(fd1, msg, sizeof(msg));
    //}

    //close(fd1);
    //close(fd2);
    //close(fd3);

    return 0;
}
