#include <stdio.h>

size_t factorial(size_t num)
{
    size_t sum = 1;
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