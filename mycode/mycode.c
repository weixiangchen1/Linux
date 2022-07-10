#include <stdio.h>

int Sum(int n)
{
    int i = 0;
    int sum = 0;
    for(i=0; i<=n; i++)
    {
        sum += i;
    }

    return sum;
}

int main()
{
    int n = 100;
    int result = Sum(n);

    printf("sum = %d\n", result);

    return 0;
}
