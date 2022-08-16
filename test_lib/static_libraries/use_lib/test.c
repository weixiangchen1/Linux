#include "add.h"
#include "sub.h"
int main()
{
    int x = 10;
    int y = 20;

    int res1 = my_add(x,y);
    int res2 = my_sub(x,y);

    printf("add: %d\n", res1);
    printf("sub: %d\n", res2);
    return 0;
}