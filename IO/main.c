#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    const char* msg = "call write\n";
    write(1, msg, strlen(msg));
    printf("call printf\n");
    fprintf(stdout, "call fprintf\n");
    fputs("call fputs\n", stdout);
    fflush(stdout);

    fork();
    return 0;
}

// int main()
// {    
//     close(1); // 关闭标准输入
//     int fd = open("./myfile", O_WRONLY | O_CREAT, 0644);
//     if(fd < 0)
//     {
//         perror("open");
//         return 1;
//     } 

//     printf("fd: %d\n", fd);
//     fprintf(stdout, "hello world\n");
//     fprintf(stdout, "hello world\n");
//     fprintf(stdout, "hello world\n");
//     fprintf(stdout, "hello world\n");

//     fflush(stdout);
//     close(fd);
//     return 0;
// }

// int main()
// {
//     int fd = open("./myfile", O_WRONLY | O_CREAT, 0664);
//     if(fd < 0){
//         perror("open");
//         return 1;
//     }

//     close(1);
//     dup2(fd, 1); // 将1号文件描述符变成fd，实现输出重定向
//     int cnt = 5;
//     const char* msg = "hello world\n"; 
//     while(cnt--){
//         write(1, msg, strlen(msg));
//     }

//     return 0;
// }

// int main()
// {
//     close(1);
//     int fd = open("./myfile", O_WRONLY | O_CREAT, 0664);
//     if(fd < 0){
//         perror("open");
//         return 1;
//     }

//     printf("fd: %d\n", fd);
//     fflush(stdout);
//     close(fd);

//     return 0;
// }

// int main()
// {
//     int fd = open("./myfile", O_RDONLY);
//     if(fd < 0){
//         perror("open");
//         return 1;
//     }

//     char buffer[1024];
//     ssize_t s = 0;
//     while(s = read(fd, buffer, sizeof(buffer)))
//     {
//         buffer[s] = 0;
//         printf("%s", buffer);
//     }

//     close(fd);

//     return 0;
// }

// int main()
// {
//     int fd = open("./myfile", O_WRONLY | O_CREAT, 0664);
//     if(fd < 0){
//         perror("open");
//         return 1;
//     }

//     int cnt = 5;
//     const char* str = "hello Linux\n";
//     while(cnt--){
//         write(fd, str, strlen(str));
//     }

//     close(fd);
    
//     return 0;
// }


// int main()
// {
//     const char* msg = "hello world\n";
//     printf("%s", msg);
//     fprintf(stdout, msg);
//     fwrite(msg, strlen(msg), 1, stdout);

//     return 0;
// }


// int main()
// {   
//     FILE* fp = fopen("myfile", "r");
//     if(fp == NULL){
//         printf("open file error\n");
//     }
//     ssize_t s = 0;
//     char* buffer[1024];
//     while(s = fread(buffer, 1, sizeof(buffer), fp))
//     {
//         buffer[s] = 0;
//         printf("%s", buffer);
//     }

//     fclose(fp);

//     return 0;
// }


// int main()
// {   
//     FILE* fp = fopen("myfile", "w");
//     if(fp == NULL){
//         printf("open file error\n");
//     }

//     // 打开文件成功，开始写文件
//     const char* str = "hello world\n";
//     int cnt = 5;
//     while(cnt--)
//     {
//         fwrite(str, strlen(str), 1, fp);
//     }

//     fclose(fp);

//     return 0;
// }