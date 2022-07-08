#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define Num 100

int main()
{
    const char* lable = "|/-\\";
    char buf[Num + 1];
    memset(buf, 0, sizeof(buf));
    int count = 0;
    while(count <= 100)
    {
        printf("[%-100s][%2d%%][%c]\r", buf, count, lable[count%4]);
        fflush(stdout);
        buf[count] = '#';
        usleep(50000);        
        
        count++;
    }
    printf("\n");

    return 0;
}
