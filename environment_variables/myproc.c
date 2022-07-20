#include <stdio.h>
#include <stdlib.h>

int main()
{
    char* env = getenv("MYENV");
    if(env){
        printf("%s\n", env);
    }
    
    return 0;
}


// 通过系统调用获取或设置环境变量
//int main()
//{
//    printf("%s\n", getenv("PATH"));
//    printf("%s\n", getenv("HOME"));
//    printf("%s\n", getenv("SHELL"));
//    printf("%s\n", getenv("USER"));
//    
//    return 0;
//}
//

// 通过第三方变量environ获取
//int main()
//{
//    extern char **environ;
//    for(int i=0; environ[i]; i++){
//        printf("environ[%d]: %s\n", i, environ[i]);
//    }
//
//    return 0;
//}
//
//

// 命令行参数
//int main(int argc, char* argv[], char* env[])
//{
//    for(int i=0; env[i]; i++)
//    {
//        printf("env[%d]: %s\n", i, env[i]);
//    }
//
//
//    return 0;
//}
