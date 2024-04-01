#include <stdio.h>

int factorial(int num)
{
    int sum = 1;
    for (; num > 1; --num)
    {
        sum *= num;
    }
    return sum;
}

int main(void)
{
    printf("factorial %d = %zu\n", 5, factorial(5));
    return 0;
}

