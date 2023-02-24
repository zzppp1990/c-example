#include<stdio.h>
/*
参考：https://blog.csdn.net/lollipop_jin/article/details/8159149
*/

int main()
{
#if 0
    float a = 0.3;
    float b = 0.6;
    float c = a * b;
    float d = a + b;

    if(c == 0.18)
        printf("c == 0.18\n");

    if(d == 0.9)
        printf("d == 0.9\n");

    printf("c = %.8f\n", c);
#endif

#if 1
    double a = 0.3;
    double b = 6;
    double a1 = 0.6;
    
    double c = a * b;
    double d = a + b;
    double e = a + a1;

    const double EPS = 1e-6; 

    if(c == 1.8)
        printf("c == 1.8\n");

    if(d == 6.3)
        printf("d == 6.3\n");

    if(e == 0.9)
        printf("e == 0.9\n");

    printf("c = %.10f\n", c);
    printf("EPS = %.10f\n", EPS);

#endif

    return 0;
}