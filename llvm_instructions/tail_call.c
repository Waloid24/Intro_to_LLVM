#include <stdio.h>

int test (int a, int b)
{
    return (a > b) ? a : b;
}

int main() {
    int a = 5, b = 7;
    return test (a, b);
}