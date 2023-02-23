#include<stdio.h>
/*
参考：https://blog.csdn.net/lollipop_jin/article/details/8159149
*/

int main()
{
    float a = 0.3;
    float b = 0.6;
    float c = a * b;
    float d = a + b;

    if(c == 0.18)
        printf("c == 0.18\n");

    if(d == 0.9)
        printf("d == 0.9\n");

    printf("c = %.8f\n", c);
    return 0;
}