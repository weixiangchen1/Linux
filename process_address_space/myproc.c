#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// 验证进程地址空间
int g_val = 10;
int main()
{
    if(fork() == 0){
        //child
        int cnt = 5;
        while(cnt){
            printf("I am child, times: %d, g_val = %d, &g_val = %p\n", cnt, g_val, &g_val);
            cnt--;
            sleep(1);
            if(cnt == 3){
                printf("#################child更改数据###################");
                g_val = 20;
                printf("###############child更改数据完成#################");
            }
        }
    }
    else{
        // parent
      while(1){
        printf("I am parent, g_val = %d, &g_val = %p\n", g_val, &g_val);
        sleep(1);
      }
    }
      
    return 0;
}


// 验证进程地址空间布局
//int g_unval;
//int g_val = 100;
//int main()
//{
//    const char* str = "hello world";
//    printf("code addr: %p\n", main);
//    printf("string rdonly addr: %p\n", str);
//    printf("init addr: %p\n", &g_val);
//    printf("uninit addr: %p\n", &g_unval);
//    char* heap = (char*)malloc(10);
//    printf("heap addr: %p\n", heap);
//    printf("stack addr: %p\n", &str);
//    printf("stack addr: %p\n", &heap);
//    
//    return 0;
//}
