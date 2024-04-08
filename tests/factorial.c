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
    printf("factorial %d = %d\n", 5, factorial(5));

    printf("factorial %d = %d\n", 2, factorial(2));
    return 0;
}

