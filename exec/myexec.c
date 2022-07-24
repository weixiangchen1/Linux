#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    if(fork() == 0){
         // child
       
         // 测试execvp
         //char *argv[] = {
         //       "ps", "ajx", NULL 
         //};
         //execvp("ps", argv);
 
         // 测试execv
         //char *argv[] = {
         //    "ls", "-a", "-l", "-d", NULL
         //};
         //execv("/usr/bin/ls", argv);
  
         // 测试execlp
         //execlp("ls", "ls", "-l", "-a", "-i", NULL);
         
         // 测试execl
         //execl("/usr/bin/ls", "ls", "-l", "-a", NULL);
         exit(1); // 替换失败返回信号1
    }
     
    // parent
    waitpid(-1, NULL, 0);
    printf("wait success\n");


    return 0;
}
