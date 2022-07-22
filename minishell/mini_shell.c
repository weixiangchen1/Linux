#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
  
#define NUM 128
#define CMD_NUM 64

int main()
{
    char command[NUM];
    for( ; ; ){
        char *argv[CMD_NUM] = {NULL};
        command[0] = '\0';
        // 1.打印提示符
        printf("[cwx@localhost dir]$ ");
        fflush(stdout);

        // 2.获取字符串命令
        command[strlen(command)-1] = '\0';
        fgets(command, NUM, stdin);

        // 3.解析字符串命令
        int i = 1;
        const char *sep = " ";
        argv[0] = strtok(command, sep);
        while(argv[i] = strtok(NULL, sep)){
            i++;
        }

        // 5.执行第三方命令
        if(fork() == 0){
            execvp(argv[0], argv);
            exit(1);
        }

    } 
    

    return 0;
}
